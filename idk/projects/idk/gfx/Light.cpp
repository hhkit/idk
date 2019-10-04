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
			const vec3 v_pos = light->GetGameObject()->Transform()->GlobalPosition();
			const vec3 v_dir = light->GetGameObject()->Transform()->Forward();
			return invert_rotation(look_at(v_pos, v_pos + v_dir, vec3{ 0,1,0 }));
		}
		mat4 operator()(DirectionalLight)
		{
			const vec3 v_pos = light->GetGameObject()->Transform()->GlobalPosition();
			const vec3 v_dir = light->GetGameObject()->Transform()->Forward();
			//return look_at(-vec3(v_dir.x,v_dir.y,-v_dir.z).normalize(), vec3(0,0,0), vec3{ 0,1,0 });
			return look_at(v_pos, v_pos + vec3(v_dir.x, v_dir.y, -v_dir.z), vec3{ 0,1,0 });
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
		mat4 operator()(DirectionalLight)
		{
			return ortho(-10.f, 10.f, -10.f, 10.f,-10.f, 10.f);//perspective(spotlight.outer_angle, 1.0f, 0.1f, 1/spotlight.attenuation_radius);
		}
		template<typename T>
		mat4 operator()(T&) { return mat4{}; }
	};

	RscHandle<RenderTarget>& Light::GetLightMap()
	{
		// TODO: insert return statement here
		return
			std::visit([&](auto& light_variant) ->RscHandle<RenderTarget> &
				{
					return light_variant.light_map;
				}
		, light);
	}
	const RscHandle<RenderTarget>& Light::GetLightMap() const
	{
		// TODO: insert return statement here
		return
			std::visit([&](auto& light_variant)-> const RscHandle<RenderTarget> &
				{
					return light_variant.light_map;
				}
		, light);
	}
	void Light::SetLightMap(const RscHandle<RenderTarget>& light_map)
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
					retval.light_color = point_light.light_color * point_light.intensity;
					retval.v_pos = GetGameObject()->Transform()->GlobalPosition();
					retval.intensity = point_light.intensity;
					//vp = ortho() * look_at(retval.v_pos, retval.v_pos + retval.v_dir, vec3{ 0,1,0 });
				}

				if constexpr (std::is_same_v<T, DirectionalLight>)
				{
					const DirectionalLight& dir_light = light_variant;
					retval.light_color = dir_light.light_color * dir_light.light_color;
					const auto tfm = GetGameObject()->Transform();
					retval.v_pos = tfm->GlobalPosition();
					retval.v_dir = tfm->Forward();
					retval.intensity = dir_light.intensity;
				}

				if constexpr (std::is_same_v<T, SpotLight>)
				{
					const SpotLight& spotlight = light_variant;
					retval.light_color = spotlight.light_color * spotlight.light_color;
					retval.v_pos = GetGameObject()->Transform()->GlobalPosition();
					retval.v_dir = GetGameObject()->Transform()->Forward();
					retval.cos_inner = cos(spotlight.inner_angle);
					retval.cos_outer = cos(spotlight.outer_angle);
					retval.intensity = spotlight.intensity;
					//vp = :spotlight.attenuation_radius)*look_at(retval.v_pos, retval.v_pos + retval.v_dir, vec3{ 0,1,0 });
				}
				retval.shadow_bias = shadow_bias;
				retval.v = LightCameraView{ this }(light_variant);
				retval.p = LightCameraProj{}(light_variant);
				retval.vp = retval.p * retval.v;
				retval.light_map = light_variant.light_map;
				retval.cast_shadow = casts_shadows;
			}
		, light);

		return retval;
	}
	CameraData Light::GenerateCameraData() const
	{
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