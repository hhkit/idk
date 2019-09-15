#include "stdafx.h"
#include "GfxDbgTest.h"
#include <idk.h>
#include <gfx/DebugRenderer.h>
#include <math/matrix_transforms.h>
#include <gfx/GraphicsSystem.h>
namespace idk
{
	//TODO remove once merged with master (26/8/19)
	/*mat4 look_at(vec3 const& eye, vec3 const& center, vec3 const& up)
	{

		vec3 const f((center - eye).get_normalized());
		vec3 const s((f.cross(up)).get_normalized());
		vec3 const u(s.cross(f));

		mat4 Result{};
		Result[0][0] = s.x;
		Result[1][0] = s.y;
		Result[2][0] = s.z;
		Result[0][1] = u.x;
		Result[1][1] = u.y;
		Result[2][1] = u.z;
		Result[0][2] = -f.x;
		Result[1][2] = -f.y;
		Result[2][2] = -f.z;
		Result[3][0] = -dot(s, eye);
		Result[3][1] = -dot(u, eye);
		Result[3][2] = dot(f, eye);
		return Result;
	}*/
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
		auto t = tan(fov / 2);

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
		auto target = (object- eye).normalize();
		auto right = target.cross(global_up).normalize();
		auto up = right.cross(target).normalize();

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
			uint32_t width = 1280;
			uint32_t height = 720;


			//Handle<Camera> currCamera = Core::GetSystem<GraphicsSystem>().CurrentCamera();
			static vec3 lookat_offset{ 0,0,5.0f };
			static vec3 lookat_offset2{ 0,0,-2.5f };

			vec3 diff = (vec3{}-lookat_offset).get_normalized();
			vec3 axis = vec3{ 0,0,1 }.cross(diff).get_normalized();

			auto angle = -acos(-diff.z);
			mat4 view = //translate(lookat_offset)*mat4 { rotate(axis, angle) };//
			//view = view.inverse();
			look_at2(lookat_offset, vec3{ 0,0,0.0f }, vec3{ 0,1,0 });
			mat4 proj =  perspective2(idk::rad(45.0f), width / (float)height, 0.001f, 20.0f);// translate(vec3{ 0,0,0.5f })* perspective(idk::rad(45.0f), width / (float)height, 0.1f, 20.0f);
			//proj[2][3] *= -1;
			static DebugObject tmp{ DbgShape::eCube };
			tmp.pos = (tmp.pos.x < 1) ? tmp.pos + vec3{ 0.005f,0,0 } : vec3{};
			tmp.angle = (tmp.angle.value < pi) ? tmp.angle + rad{ 0.001f } : rad{ 0 };
			lookat_offset = rotate(vec3{ 0,1,0 }, rad{ 0.005f }) * lookat_offset;
			DebugObject orbit{ DbgShape::eCube };



			//e1_l* l2g = forward;


			lookat_offset2 = orbit.pos = rotate(vec3{ 0,1,0 }, rad{ 0.01f })  * lookat_offset2;
			vec3 diff2 = Normalized(vec3{}-lookat_offset2);
			vec3 axis2 = Normalized(vec3{ 0,0,1 }.cross(diff2));
			auto angle2 = acos(diff2.z);
			orbit.Rotation(axis2, angle2);
			dbg_renderer.DrawShape(orbit);
			dbg_renderer.DrawShape(tmp);
			int max_count = 10;
			for (int i = 0; i < max_count; ++i)
			{
				float chunk = 2.0f / max_count;
				dbg_renderer.DrawShape((i % 2) ? DbgShape::eSquare : DbgShape::eCube, vec3{ -1.0f + chunk * i,0,0 }, vec3{ chunk,chunk,chunk }, vec3{ 0,1,1 }, rad{ 0*pi * chunk * i }, vec4{ 0,1,0,1 });

			}
			//dbg_renderer.DrawShape(DbgShape::eSquare, vec3{ 0.2f,0.4f,0 }, vec3{ 0.2f,0.2f,1 }, vec3{ 0,0,1 }, rad{ pi / 6 }, vec4{ 0,1,0,1 });
			//dbg_renderer.DrawShape(DbgShape::eSquare, vec3{ 0.1f,0,0 }, vec3{ 1,1,1 }, vec3{ 0,0,1 }, rad{ pi / 2 }, vec4{ 0,1,0,1 });
			//dbg_renderer.DrawShape(DbgShape::eSquare, vec3{ 0.3f,-0.3f,0 }, vec3{ 1,1,1 }, vec3{ 0,0,1 }, rad{ pi / 5 }, vec4{ 0,1,0,1 });
			//dbg_renderer.DrawShape(DbgShape::eSquare, vec3{ -0.3f,0.2f,0 }, vec3{ 1,1,1 }, vec3{ 0,0,1 }, rad{ pi / 4 }, vec4{ 0,1,0,1 });
			dbg_renderer.DrawShape(DebugObject{ DbgShape::eEqTriangle }.Scale(vec3{ 0.5f,0.5f,0.5f }).Rotation(vec3{ 0,0,1 }, rad{ 0 * pi / 3 }).Color(vec4{ 0,1,0,1 }));
			dbg_renderer.Render(view, proj);
		}
	}

}