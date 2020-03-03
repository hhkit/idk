#include "pch.h"
#include "ShadowBindings.h"
namespace idk::vkn::bindings
{

	bool ShadowFilter::Skip(RenderInterface&, const RenderObject& dc)
	{
		return !(dc.layer_mask & filter);
	}

	void ShadowFilter::SetState(const CameraData& cam)
	{
		filter = cam.culling_flags;
	}
	void GeometryShaderBinding::Bind(RenderInterface& the_interface)
	{
		the_interface.BindShader(ShaderStage::Geometry, geometry_shader);
	}
}