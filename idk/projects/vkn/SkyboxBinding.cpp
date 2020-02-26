#include "pch.h"
#include "SkyboxBinding.h"
#include <gfx/GraphicsSystem.h>
#include <vkn/VulkanMesh.h>
#include <vkn/BufferHelpers.inl>
#include <vkn/RenderUtil.h>
//#include <math/matrix_transforms.inl>
//bool Skip(RenderInterface&, const  RenderObject&) override { return false; }
//Stuff that should be bound at the start, before the renderobject/animated renderobject loop.
namespace idk::vkn::bindings
{
	bool SkyboxBindings::Skip() const
	{
		return _camera.clear_data.index() != meta::IndexOf_v<decltype(_camera.clear_data), RscHandle<CubeMap>>;
	}
	bool SkyboxBindings::Skip(RenderInterface& , const RenderObject& )
	{
		return Skip();
	}
	void SkyboxBindings::Bind(RenderInterface& the_interface)
	{
		the_interface.BindShader(ShaderStage::Vertex, Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VSkyBox]);
		the_interface.BindShader(ShaderStage::Fragment, Core::GetSystem<GraphicsSystem>().renderer_fragment_shaders[FSkyBox]);
		the_interface.SetScissorsViewport(_camera.viewport);
		the_interface.SetCullFace(CullFace::eNone);
		the_interface.SetDepthWrite(false);
		the_interface.SetDepthTest(false);
		if (Skip())
			BindClear(the_interface);
	}

	//Stuff that needs to be bound with every renderobject/animated renderobject
	void SkyboxBindings::Bind(RenderInterface& the_interface, [[maybe_unused]] const RenderObject& dc)
	{
		auto& camera = _camera;
		auto vp = camera.projection_matrix * mat4{ mat3{ camera.view_matrix } };
		auto vvp = mat4{ 1,0,0,0,
										0,1,0,0,
										0,0,0.5f,1.0f,
										0,0,0,1 }*vp;
		auto mat4block = FakeMat4{ vp };
		auto sb_cm = std::get<RscHandle<CubeMap>>(camera.clear_data);
		the_interface.BindUniform("sb", 0, sb_cm.as<VknCubemap>());
		the_interface.BindUniform("CameraBlock", 0, string_view{ hlp::buffer_data<const char*>(mat4block),hlp::buffer_size(mat4block) });
	}
	struct ClearBinder
	{
		RenderInterface& the_interface;
		template<typename T>
		void operator()(T&& t) {} //Catchall

		void operator()(color col)
		{
			the_interface.SetClearColor(0, col);
		}
	};
	void SkyboxBindings::BindClear(RenderInterface& the_interface) const
	{
		ClearBinder clear{ the_interface };
		std::visit(clear, _camera.clear_data);
	}

}