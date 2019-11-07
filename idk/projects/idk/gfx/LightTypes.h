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
		bool  use_inv_sq_atten   { true };

		RscHandle<FrameBuffer> light_map;

		const void* unique_id() const noexcept;
		RscHandle<FrameBuffer> InitShadowMap();
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
	};

	bool NeedShadowMap(const PointLight&);
	bool NeedShadowMap(const SpotLight&);
	bool NeedShadowMap(const DirectionalLight&);
	using LightVariant = variant<PointLight, DirectionalLight, SpotLight>;
}