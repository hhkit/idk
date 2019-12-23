#include "stdafx.h"
#include "GfxDbgTest.h"
#include <idk.h>
#include <gfx/DebugRenderer.h>
#include <math/matrix_transforms.h>
#include <gfx/GraphicsSystem.h>
#include <math/shapes.h>
#include <math/angle.inl>

namespace idk
{
	template<typename T>
	tmat<T, 4, 4> perspective2(trad<T> fov, T a, T n, T f)
	{
		//return 
			mat4{
			1,0,0,0,
			0,-1,0,0,
			0,0,0.5f,0.5f,
			0,0,0,1
		}*perspective(fov, a, n, f);
		const auto t = tan(fov / 2);

		constexpr auto _2 = s_cast<T>(2);
		constexpr auto _1 = s_cast<T>(1);
		constexpr auto _0 = s_cast<T>(0);

		return tmat<T, 4, 4>{
			_1 / (t * a), _0, _0, _0,
				_0, _1 / t, _0, _0,
				_0, _0,  f/ (n - f), (f*n) / (n -f),
				_0, _0, -_1, _0
		};
	}
	template<typename Vec>
	Vec Normalized(const Vec& vec)
	{
		return vec / vec.length();
	}
	template<typename T>
	tmat<T, 4, 4> look_at2(const tvec<T, 3> & eye, const tvec<T, 3> & object, const tvec<T, 3> & global_up)
	{
		const auto target = (object- eye).normalize();
		const auto right = target.cross(global_up).normalize();
		const auto up = right.cross(target).normalize();

		return tmat<T, 4, 4>{
			vec4{ right,  0 },
				vec4{ up,     0 },
				vec4{ target, 0 },
				vec4{ eye.dot(right),eye.dot(up),eye.dot(target),    1 }
		};
	}
	void GfxDebugTest()
	{
		auto& dbg_renderer = Core::GetSystem<DebugRenderer>();
		if (&dbg_renderer)
		{
			const uint32_t width = 1280;
			const uint32_t height = 720;


			//Handle<Camera> currCamera = Core::GetSystem<GraphicsSystem>().CurrentCamera();
			static vec3 lookat_offset{ 0,0,5.0f };
			static vec3 lookat_offset2{ 0,0,-2.5f };

			const vec3 diff = (vec3{}-lookat_offset).get_normalized();
			const vec3 axis = vec3{ 0,0,1 }.cross(diff).get_normalized();

			const auto angle = -acos(-diff.z);
			const mat4 view =  look_at2(lookat_offset, vec3{ 0,0,0.0f }, vec3{ 0,1,0 });
			const mat4 proj =  perspective2(idk::rad(45.0f), width / (float)height, 0.001f, 20.0f);

			const box tmp = [angle]()
			{
				box retval;
				retval.center = (retval.center.x < 1) ? retval.center + vec3{ 0.005f,0,0 } : vec3{};
				const rad t_angle = (angle.value < pi) ? angle + rad{ 0.001f } : rad{ 0 };
				retval.rotation = quat{ vec3{1,0,0}, t_angle };
				return retval;
			}();

			lookat_offset = rotate(vec3{ 0,1,0 }, rad{ 0.005f }) * vec4(lookat_offset, 0);

			const box orbit = [&]()
			{
				box retval;
				lookat_offset2 = retval.center = rotate(vec3{ 0,1,0 }, rad{ 0.01f }) * vec4 { lookat_offset2, 0 };
				const vec3 diff2 = Normalized(-lookat_offset2);
				const vec3 axis2 = Normalized(vec3{ 0,0,1 }.cross(diff2));
				const auto angle2 = acos(diff2.z);
				retval.rotation = quat{ axis2, angle2 };

				return retval;
			}();

			dbg_renderer.Draw(orbit);
			dbg_renderer.Draw(tmp);
			const int max_count = 10;
			for (int i = 0; i < max_count; ++i)
			{
				float chunk = 2.0f / max_count;
				if (i % 2)
				{
					// can't debug square
				}
				else
				{
					const box b = [&]()
					{
						box retval;
						retval.center = vec3{ -1.0f + chunk * i,0,0 };
						retval.extents = vec3{ chunk,chunk,chunk };
						retval.rotation = quat{ vec3{0,1,1},  rad{ pi * chunk * i } };
						return retval;
					}();
					
					dbg_renderer.Draw(b, color{ 0,1,0 });
				}
			}
			// can't debug triangle
		}
	}

}