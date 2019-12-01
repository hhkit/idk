#pragma once
#include <idk.h>
#include <res/ResourceHandle.h>

#undef near
#undef far

namespace idk
{
	class FrameBuffer;

	struct PointLight
	{
		real  intensity          { 1.f } ;
		color light_color        { 1.f } ;
		real  attenuation_radius { 1.f } ;
		bool  use_inv_sq_atten{ true };

		RscHandle<FrameBuffer> light_map;

		const void* unique_id() const noexcept;
		RscHandle<FrameBuffer> InitShadowMap();
		
		real GetAttenuationRadius() const { return attenuation_radius; }
		void SetAttenuationRadius(const real& val) { attenuation_radius = val; }
		bool GetInvSqAtten() const { return use_inv_sq_atten; }
		void SetInvSqAtten(const bool& val) { use_inv_sq_atten = val; }
	};

	struct DirectionalLight
	{
		real  intensity     { 1.f  };
		color light_color   { 1.f  };
		real  left          { -5.0f };
		real  right         { +5.0f };
		real  bottom        { -5.0f };
		real  top           { +5.0f };
		real  near          { -5.0f };
		real  far           { +5.0f };

		RscHandle<FrameBuffer> light_map;
		RscHandle<FrameBuffer> InitShadowMap();
		const void* unique_id()const noexcept;
	};

	struct SpotLight
	{
		real  intensity          { 1.f };
		color light_color        { 1.f };
		rad   inner_angle        { 0.f };
		rad   outer_angle        { half_pi/2 };
		real  attenuation_radius { 1.f };
		bool  use_inv_sq_atten   { true };

		RscHandle<FrameBuffer> light_map;
		RscHandle<FrameBuffer> InitShadowMap();
		const void* unique_id()const noexcept;

		real GetAttenuationRadius() const { return attenuation_radius; }
		void SetAttenuationRadius(const real& val) { attenuation_radius = val; }
		bool GetInvSqAtten() const { return use_inv_sq_atten; }
		void SetInvSqAtten(const bool& val) { use_inv_sq_atten = val; }
	};

	bool NeedShadowMap(const PointLight&);
	bool NeedShadowMap(const SpotLight&);
	bool NeedShadowMap(const DirectionalLight&);
	using LightVariant = variant<PointLight, DirectionalLight, SpotLight>;
}