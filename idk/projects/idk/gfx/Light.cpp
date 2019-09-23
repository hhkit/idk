#include "stdafx.h"
#include "Light.h"
#include <core/GameObject.h>
#include <common/Transform.h>

namespace idk
{
	RscHandle<LightMap>& Light::GetLightMap()
	{
		// TODO: insert return statement here
		return
			std::visit([&](auto& light_variant) ->RscHandle<LightMap> &
				{
					return light_variant.light_map;
				}
		, light);
	}
	const RscHandle<LightMap>& Light::GetLightMap() const
	{
		// TODO: insert return statement here
		return
			std::visit([&](auto& light_variant)-> const RscHandle<LightMap> &
				{
					return light_variant.light_map;
				}
		, light);
	}
	void Light::SetLightMap(const RscHandle<LightMap>& light_map)
	{
		GetLightMap() = light_map;
	}
	LightData Light::GenerateLightData() const
	{
		LightData retval;
		retval.index = (int) light.index();
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
				retval.light_map = light_variant.light_map;
			}
		, light);

		return retval;
	}
	CameraData Light::GenerateCameraData() const
	{
		mat4 view_matrix;
		mat4 projection_matrix;
		return CameraData{
			0xFFFFFFF,
			view_matrix,
			projection_matrix,
			std::visit([&](auto& light_variant)
			{
				return light_variant.light_map;
			}
		, light),
			false,
			vec4{1,1,1,1},
			true
		};
	}
}