#include "stdafx.h"
#include "Light.h"
#include <core/GameObject.h>
#include <common/Transform.h>

namespace idk
{
	LightData Light::GenerateLightData() const
	{
		LightData retval;
		retval.index = light.index();
		std::visit([&](auto& light_variant)
			{
				using T = std::decay_t<decltype(light_variant)>;
				if constexpr (std::is_same_v<T, PointLight>)
				{
					const PointLight& point_light = light_variant;
					retval.light_color = point_light.light_color;
					retval.v_pos = GetGameObject()->Transform()->GlobalPosition();
				}

				if constexpr (std::is_same_v<T, DirectionalLight>)
				{
					const DirectionalLight& dir_light = light_variant;
					retval.light_color = dir_light.light_color;
					retval.v_dir = GetGameObject()->Transform()->Forward();
				}

				if constexpr (std::is_same_v<T, SpotLight>)
				{
					const SpotLight& spotlight = light_variant;
					retval.light_color = spotlight.light_color;
					retval.v_pos = GetGameObject()->Transform()->GlobalPosition();
					retval.v_dir = GetGameObject()->Transform()->Forward();
					retval.cos_inner = cos(spotlight.inner_angle);
					retval.cos_outer = cos(spotlight.outer_angle);
				}
			}
		, light);

		return retval;
	}
}