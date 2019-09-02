#pragma once
#include <idk.h>
#include <res/Resource.h>

namespace idk
{
	class LightMap;

	struct PointLight
	{
		real  intensity          { 1.f };
		color light_color        { 1.f };
		real  attenuation_radius { 1.f };
		bool  use_inv_sq_atten   { true };

		RscHandle<LightMap> lightmap;
	};

	struct DirectionalLight
	{
		real  intensity     { .5f  };
		color light_color   { 1.f  };

		RscHandle<LightMap> lightmap;
	};

	struct SpotLight
	{
		real  intensity          { 1.f };
		color light_color        { 1.f };
		rad   inner_angle        { 0.f };
		rad   outer_angle        { half_pi };
		real  attenuation_radius { 1.f };
		bool  use_inv_sq_atten   { true };

		RscHandle<LightMap> lightmap;
	};

	using LightVariant = variant<PointLight, DirectionalLight, SpotLight>;
}