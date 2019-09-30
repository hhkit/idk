#include "stdafx.h"
#include "Light.h"
#include <core/GameObject.h>
#include <common/Transform.h>
#include <math/matrix_transforms.h>

namespace idk
{
	struct LightCameraView
	{
		const Light* light;
		mat4 operator()(SpotLight )
		{
			vec3 v_pos = light->GetGameObject()->Transform()->GlobalPosition();
			vec3 v_dir = light->GetGameObject()->Transform()->Forward();
			return look_at(v_pos, v_pos + v_dir, vec3{ 0,1,0 });
		}
		template<typename T>
		mat4 operator()(T&) { return mat4{}; }
	};
	struct LightCameraProj
	{
		mat4 operator()(SpotLight spotlight)
		{
			return perspective(spotlight.outer_angle, 1.0f, 0.1f, (spotlight.use_inv_sq_atten) ? (1 / spotlight.attenuation_radius) : spotlight.attenuation_radius);;//perspective(spotlight.outer_angle, 1.0f, 0.1f, 1/spotlight.attenuation_radius);
		}
		template<typename T>
		mat4 operator()(T&) { return mat4{}; }
	};

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
				mat4 vp;
				using T = std::decay_t<decltype(light_variant)>;
				if constexpr (std::is_same_v<T, PointLight>)
				{
					const PointLight& point_light = light_variant;
					retval.light_color = point_light.light_color;
					retval.v_pos = GetGameObject()->Transform()->GlobalPosition();
					//vp = ortho() * look_at(retval.v_pos, retval.v_pos + retval.v_dir, vec3{ 0,1,0 });
				}

				if constexpr (std::is_same_v<T, DirectionalLight>)
				{
					const DirectionalLight& dir_light = light_variant;
					retval.light_color = dir_light.light_color;
					retval.v_dir = GetGameObject()->Transform()->Forward();
					//TODO: Grab potential shadow receivers and caster before generating the approrpriate sized map
					//vp = ortho() * look_at(retval.v_pos, retval.v_pos + retval.v_dir, vec3{ 0,1,0 });
				}

				if constexpr (std::is_same_v<T, SpotLight>)
				{
					const SpotLight& spotlight = light_variant;
					retval.light_color = spotlight.light_color;
					retval.v_pos = GetGameObject()->Transform()->GlobalPosition();
					retval.v_dir = GetGameObject()->Transform()->Forward();
					retval.cos_inner = cos(spotlight.inner_angle);
					retval.cos_outer = cos(spotlight.outer_angle);
					
					//vp = :spotlight.attenuation_radius)*look_at(retval.v_pos, retval.v_pos + retval.v_dir, vec3{ 0,1,0 });
				}
				retval.vp = LightCameraProj{}(light_variant) *LightCameraView{ this }(light_variant);
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
			std::visit(LightCameraView{ this },light),
			std::visit(LightCameraProj{ },light),
			std::visit([&](auto& light_variant)
			{
				return light_variant.light_map;
			}
		, light),
			false,
			true,
			vec4{1,1,1,1}
		};
	}
}