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
	void GfxDebugTest()
	{
		auto& dbg_renderer = Core::GetSystem<DebugRenderer>();
		if (&dbg_renderer)
		{
			uint32_t width = 1280;
			uint32_t height = 720;


			//Handle<Camera> currCamera = Core::GetSystem<GraphicsSystem>().CurrentCamera();

			mat4 view = look_at(vec3{ 0,2,2 }, vec3{ 0,0,0 }, vec3{ 0,1,0 });
			mat4 proj = perspective(idk::rad(45.0f), width / (float)height, 0.1f, 10.0f);
			static DebugObject tmp{ DbgShape::eCube };
			tmp.pos = (tmp.pos.x < 1) ? tmp.pos + vec3{ 0.005f,0,0 } : vec3{};
			tmp.angle = (tmp.angle.value < pi) ? tmp.angle + rad{ 0.001f } : rad{ 0 };

			dbg_renderer.DrawShape(tmp);
			int max_count = 10;
			for (int i = 0; i < max_count; ++i)
			{
				float chunk = 2.0f / max_count;
				dbg_renderer.DrawShape((i % 2) ? DbgShape::eSquare : DbgShape::eCube, vec3{ -1.0f + chunk * i,0,0 }, vec3{ chunk,chunk,chunk }, vec3{ 0,1,1 }, rad{ pi * chunk * i }, vec4{ 0,1,0,1 });

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