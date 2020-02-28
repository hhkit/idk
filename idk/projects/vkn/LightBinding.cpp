#include "pch.h"
#include "LightBinding.h"
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>
#include <ds/result.inl>
namespace idk::vkn::bindings
{
//#pragma optimize ("",off)
	bool DeferredAmbientLight::Skip(RenderInterface& context, const RenderObject&)
	{
		return !fragment_shader.as<ShaderModule>().HasCurrent();
	}
	void DeferredAmbientLight::Bind(RenderInterface& context)
	{

		auto irradiance_map =
			(camera.clear_data.index() == meta::IndexOf<CameraClear, RscHandle<CubeMap>>::value)
			? RscHandle<CubeMap>{std::get<RscHandle<CubeMap>>(camera.clear_data).as<VknCubemap>().GetConvoluted() } : RscHandle <CubeMap>{}
		;
		auto environment_probe =
			(camera.clear_data.index() == meta::IndexOf<CameraClear, RscHandle<CubeMap>>::value)
			? std::get<RscHandle<CubeMap>>(camera.clear_data) : RscHandle < CubeMap>{}
		;
		context.BindShader(ShaderStage::Vertex, Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VFsq]);
		context.BindShader(ShaderStage::Fragment, fragment_shader);
		context.BindUniform("irradiance_probe", 0, irradiance_map.as<VknCubemap>().Tex().as<VknTexture>());
		context.BindUniform("environment_probe", 0, environment_probe.as<VknCubemap>().Tex().as<VknTexture>());

		context.BindUniform("brdfLUT", 0, brdf_lut.as<VknTexture>());
		FakeMat4 inverse_view = camera.view_matrix.inverse();
		context.BindUniform("PBRBlock", 0, hlp::to_data(inverse_view));
	}
	DeferredAmbientLight::DeferredAmbientLight() : fragment_shader{Core::GetSystem<GraphicsSystem>().renderer_fragment_shaders[FDeferredPostAmbient]}
	{
		auto& derp = *fragment_shader;
	}

	void DeferredLightFsq::Bind(RenderInterface& context)
	{

		auto irradiance_map =
			(camera.clear_data.index() == meta::IndexOf<CameraClear, RscHandle<CubeMap>>::value)
			? RscHandle<CubeMap>{std::get<RscHandle<CubeMap>>(camera.clear_data).as<VknCubemap>().GetConvoluted() } : RscHandle <CubeMap>{}
		;
		auto environment_probe =
			(camera.clear_data.index() == meta::IndexOf<CameraClear, RscHandle<CubeMap>>::value)
			? std::get<RscHandle<CubeMap>>(camera.clear_data) : RscHandle < CubeMap>{}
		;
		context.BindShader(ShaderStage::Vertex, Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VFsq]);
		context.BindShader(ShaderStage::Fragment, fragment_shader);
		context.BindUniform("irradiance_probe", 0, irradiance_map.as<VknCubemap>().Tex().as<VknTexture>());
		context.BindUniform("environment_probe", 0, environment_probe.as<VknCubemap>().Tex().as<VknTexture>());

		context.BindUniform("brdfLUT", 0, brdf_lut.as<VknTexture>());
		FakeMat4 inverse_view = camera.view_matrix.inverse();
		context.BindUniform("PBRBlock", 0, hlp::to_data(inverse_view));
	}

}
