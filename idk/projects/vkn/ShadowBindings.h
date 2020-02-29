#pragma once
#include <vkn/RenderBindings.h>
#include <vkn/CameraViewportBindings.h>
#include <vkn/StandardVertexBindings.h>
namespace idk::vkn::bindings
{
	struct ShadowFilter :RenderBindings
	{
		LayerMask filter;
		bool Skip(RenderInterface& the_interface, const  RenderObject& dc) override;
		void SetState(const CameraData& cam);
	};
	using ShadowBinding = CombinedBindings<ShadowFilter, CameraViewportBindings,VertexShaderBinding,StandardVertexBindings>;
}