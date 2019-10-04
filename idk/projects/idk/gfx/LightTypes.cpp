#include "stdafx.h"
#include "LightTypes.h"
#include <gfx/RenderTarget.h>
//#include "LightTypes.h"
namespace idk
{
	const void* PointLight::unique_id() const
	{
		static const char test[] = "PointLight";
		return test;
	}
	const void* SpotLight::unique_id() const
	{
		static const char test[] = "SpotLight";
		return test;
	}
	const void* DirectionalLight::unique_id() const
	{
		static const char test[] = "DirectionalLight";
		return test;
	}
	//	SpotLight::
//		SpotLight(
//			real  ntensity          ,//= { 1.f },
//			color ight_color        ,//= color{ 1.f },
//			rad   nner_angle        ,//= { 0.f },
//			rad   uter_angle        ,//= { half_pi },
//			real  ttenuation_radius ,//= { 1.f },
//			bool  se_inv_sq_atten    //= { true }
//		):
//		intensity{ntensity},
//		light_color{ight_color},
//		inner_angle{nner_angle},
//		outer_angle{uter_angle},
//		attenuation_radius{ttenuation_radius},
//		use_inv_sq_atten{se_inv_sq_atten}
//	{
//	}
//
//	SpotLight::SpotLight(SpotLight&& rhs) noexcept
//	{
//		intensity = rhs.intensity;
//		light_color = rhs.light_color;
//		inner_angle = rhs.inner_angle;
//		outer_angle = rhs.outer_angle;
//		attenuation_radius = rhs.attenuation_radius;
//		use_inv_sq_atten = rhs.use_inv_sq_atten;
//
//		light_map = rhs.light_map;
//		rhs.light_map = RscHandle<RenderTarget>{};
//	}
//	SpotLight& SpotLight::operator=(SpotLight&& rhs) noexcept
//	{
//		intensity = rhs.intensity;
//		light_color = rhs.light_color;
//		inner_angle = rhs.inner_angle;
//		outer_angle = rhs.outer_angle;
//		attenuation_radius = rhs.attenuation_radius;
//		use_inv_sq_atten = rhs.use_inv_sq_atten;
//
//		std::swap(light_map, rhs.light_map);
//
//		return*this;
//	}
//	SpotLight::~SpotLight()
//	{
//		if (light_map)
//			Core::GetResourceManager().Free(light_map);
//		light_map = RscHandle<RenderTarget>();
//	}
//	DirectionalLight::DirectionalLight(DirectionalLight&& rhs) noexcept
//	{
//		intensity = rhs.intensity;
//		light_color = rhs.light_color;
//		light_map = rhs.light_map;
//		rhs.light_map = RscHandle<RenderTarget>{};
//	}
//	DirectionalLight& DirectionalLight::operator=(DirectionalLight&&rhs) noexcept
//	{
//		intensity = rhs.intensity;
//		light_color = rhs.light_color;
//		std::swap(light_map,rhs.light_map);
//		return *this;
//	}
//	DirectionalLight::~DirectionalLight()
//	{
//		if (light_map)
//			Core::GetResourceManager().Free(light_map);
//		light_map = RscHandle<RenderTarget>();
//	}
//	PointLight::PointLight(
//			real  in ,//= { 1.f },
//			color lc ,//= color{ 1.f },
//			real  ar ,//= { 1.f },
//			bool  inv_sq 
//	):intensity{in},light_color{lc},attenuation_radius{ar},use_inv_sq_atten{inv_sq}
//	{
//	}
//	PointLight::PointLight(PointLight&& rhs) noexcept
//	{
//		intensity = rhs.intensity;
//		light_color = rhs.light_color;
//		attenuation_radius = rhs.attenuation_radius;
//		use_inv_sq_atten = rhs.use_inv_sq_atten;
//
//		light_map =  rhs.light_map;
//		rhs.light_map = RscHandle<RenderTarget>{};
//	}
//	PointLight& PointLight::operator=(PointLight&& rhs) noexcept
//	{
//		intensity = rhs.intensity;
//		light_color = rhs.light_color;
//		attenuation_radius = rhs.attenuation_radius;
//		use_inv_sq_atten = rhs.use_inv_sq_atten;
//
//		std::swap(light_map, rhs.light_map);
//
//		return*this;
//	}
//	PointLight::~PointLight()
//	{
//		if(light_map)
//			Core::GetResourceManager().Free(light_map);
//		light_map = RscHandle<RenderTarget>();
//	}
template<typename T>
bool NeedShadowMapImpl(T& light)
{
	return !light.light_map;
}
bool NeedShadowMap(const PointLight&light){return NeedShadowMapImpl(light);};
bool NeedShadowMap(const SpotLight&light){return NeedShadowMapImpl(light);};
bool NeedShadowMap(const DirectionalLight&light){return NeedShadowMapImpl(light);};
RscHandle<RenderTarget> idk::PointLight::InitShadowMap()
{
	//TODO turn it into a cube map
	auto &shadow_map =light_map= Core::GetResourceManager().Create<RenderTarget>();
	auto m = shadow_map->GetMeta().textures[0]->GetMeta();
	m.internal_format = ColorFormat::DEPTH_COMPONENT;
	shadow_map->GetMeta().textures[0]->SetMeta(m);
	return shadow_map;
}
RscHandle<RenderTarget> DirectionalLight::InitShadowMap()
{
	auto& shadow_map = light_map = Core::GetResourceManager().Create<RenderTarget>();
	auto m = shadow_map->GetMeta().textures[0]->GetMeta();
	m.internal_format = ColorFormat::DEPTH_COMPONENT;
	shadow_map->GetMeta().textures[0]->SetMeta(m);
	return shadow_map;
}
RscHandle<RenderTarget> SpotLight::InitShadowMap()
{
	auto& shadow_map = light_map = Core::GetResourceManager().Create<RenderTarget>();
	auto m = shadow_map->GetMeta().textures[0]->GetMeta();
	m.internal_format = ColorFormat::DEPTH_COMPONENT;
	shadow_map->GetMeta().textures[0]->SetMeta(m);
	return shadow_map;
}
}

