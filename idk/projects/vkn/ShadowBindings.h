#pragma once
#include <vkn/RenderBindings.h>
#include <vkn/CameraViewportBindings.h>
#include <vkn/StandardVertexBindings.h>
namespace idk::vkn::bindings
{
	struct GeometryShaderBinding : RenderBindings
	{
		RscHandle<ShaderProgram> geometry_shader;
		void Bind(RenderInterface& the_interface)override;
	};
	struct ShadowFilter :RenderBindings
	{
		LayerMask filter;
		bool Skip(RenderInterface& the_interface, const  RenderObject& dc) override;
		void SetState(const CameraData& cam);
	};
	struct PointShadowFilter :RenderBindings
	{
		LayerMask filter;
		bool Skip(RenderInterface& the_interface, const  RenderObject& dc) override;
		void SetState(const PointCameraData& cam);
	};
	using ShadowBinding = CombinedBindings<ShadowFilter, CameraViewportBindings,VertexShaderBinding,StandardVertexBindings>;
	using PointShadowBinding = CombinedBindings<PointShadowFilter, CameraViewportBindings, VertexShaderBinding, GeometryShaderBinding, StandardVertexBindings>;
}