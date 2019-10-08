#pragma once
#include <idk.h>
#include <res/ResourceHandle.h>

namespace idk
{
	class RenderTarget;


	struct PointLight
	{
		real  intensity          { 1.f } ;
		color light_color        { 1.f } ;
		real  attenuation_radius { 1.f } ;
		bool  use_inv_sq_atten   { true };

		RscHandle<RenderTarget> light_map;

		const void* unique_id()const noexcept;

		RscHandle<RenderTarget> InitShadowMap();
		//PointLight();
		//PointLight(
		//	real  intensity         ={ 1.f },
		//	color light_color       = color{ 1.f },
		//	real  attenuation_radius={ 1.f },
		//	bool  use_inv_sq_atten  ={ true }
		//);
		////Does not copy the light_map. For serialization only.
		//PointLight(const PointLight& rhs) :PointLight{ rhs.intensity,rhs.light_color,rhs.attenuation_radius,rhs.use_inv_sq_atten} {}
		//PointLight(PointLight&&)noexcept;
		//PointLight& operator=(PointLight&&)noexcept;
		//~PointLight();
	};

	struct DirectionalLight
	{
		real  intensity     { .5f  };
		color light_color   { 1.f  };
		real  width         { 5.0f };
		real  height        { 5.0f };

		RscHandle<RenderTarget> light_map;
		RscHandle<RenderTarget> InitShadowMap();
		const void* unique_id()const noexcept;
		//DirectionalLight(
		//	real  intensity={ .5f },
		//	color light_color=color{ 1.f }
		//);
		////DirectionalLight();
		////Does not copy the light_map. For serialization only.
		//DirectionalLight(const DirectionalLight& rhs) :DirectionalLight{ rhs.intensity,rhs.light_color} {}
		//DirectionalLight(DirectionalLight&&)noexcept;
		//DirectionalLight& operator=(DirectionalLight&&)noexcept;
		//~DirectionalLight();
	};

	struct SpotLight
	{
		real  intensity          { 1.f };
		color light_color        { 1.f };
		rad   inner_angle        { 0.f };
		rad   outer_angle        { half_pi/2 };
		real  attenuation_radius { 1.f };
		bool  use_inv_sq_atten   { true };

		RscHandle<RenderTarget> light_map;
		RscHandle<RenderTarget> InitShadowMap();
		const void* unique_id()const noexcept;
		//SpotLight();
		//SpotLight(
		//	real  intensity={ 1.f },
		//    color light_color=color{ 1.f },
		//    rad   inner_angle=rad{ 0.f },
		//    rad   outer_angle=rad{ half_pi },
		//    real  attenuation_radius={ 1.f },
		//    bool  use_inv_sq_atten={ true }
		//);
		//SpotLight(const SpotLight& rhs) :SpotLight{ rhs.intensity,rhs.light_color,rhs.inner_angle,rhs.outer_angle,rhs.attenuation_radius,rhs.use_inv_sq_atten } {}
		//SpotLight(SpotLight&&)noexcept;
		//SpotLight& operator=(SpotLight&&)noexcept;
		//~SpotLight();
	};

	bool NeedShadowMap(const PointLight&);
	bool NeedShadowMap(const SpotLight&);
	bool NeedShadowMap(const DirectionalLight&);
	using LightVariant = variant<PointLight, DirectionalLight, SpotLight>;
}