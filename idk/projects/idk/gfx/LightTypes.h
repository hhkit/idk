#pragma once
#include <idk.h>
#include <res/ResourceHandle.h>

namespace idk
{
	class RenderTarget;

	struct PointLight
	{
		real  intensity          { 1.f };
		color light_color        { 1.f };
		real  attenuation_radius { 1.f };
		bool  use_inv_sq_atten   { true };

		RscHandle<RenderTarget> light_map;
	};

	struct DirectionalLight
	{
		real  intensity     { .5f  };
		color light_color   { 1.f  };

		RscHandle<RenderTarget> light_map;
	};

	struct SpotLight
	{
		real  intensity          { 1.f };
		color light_color        { 1.f };
		rad   inner_angle        { 0.f };
		rad   outer_angle        { half_pi };
		real  attenuation_radius { 1.f };
		bool  use_inv_sq_atten   { true };

		RscHandle<RenderTarget> light_map;
	};

	using LightVariant = variant<PointLight, DirectionalLight, SpotLight>;
}