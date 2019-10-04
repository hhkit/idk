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
		mat4 operator()(const SpotLight& )
		{
			auto trf = light->GetGameObject()->Transform();
			vec3 v_pos = trf->GlobalPosition();
			vec3 v_dir = trf->Forward();
			return look_at(v_pos, v_pos + v_dir, trf->Up()).inverse();
		}
		mat4 LookAt(vec3 pos, vec3 target_point,vec3 up)
		{
			//mat4 tfm;
			return look_at(pos, target_point, up);//).normalize();
		}
		mat4 View(const mat4& tfm)
		{
			auto mat =tfm;// = game_object.Transform()->GlobalMatrix();
			//auto tfm = game_object.Transform();
			auto retval = orthonormalize(mat);
			retval[3] = mat[3];

			/*vec3 upvector = tfm->Up();
			vec3 rightvector = tfm->Right();
			vec3 forwardvector = tfm->Forward();

			mat4 findMat = retval.inverse();

			mat4 matrix = mat.transpose();*/

			return retval.inverse();
		}
		mat4 operator()(const DirectionalLight&)
		{
			auto trf = light->GetGameObject()->Transform();
			vec3 v_pos = trf->GlobalPosition();
			vec3 v_dir = trf->Forward();
			//return look_at(-vec3(v_dir.x,v_dir.y,-v_dir.z).normalize(), vec3(0,0,0), vec3{ 0,1,0 });
			return look_at(v_pos, v_pos + vec3(v_dir.x, v_dir.y, v_dir.z), trf->Up());
		}
		template<typename T>
		mat4 operator()(T&) { return mat4{}; }
	};
	struct LightCameraProj
	{
		mat4 operator()(SpotLight spotlight)
		{
			return perspective(spotlight.outer_angle*2, 1.0f, 0.1f, (spotlight.use_inv_sq_atten) ? (1 / spotlight.attenuation_radius) : spotlight.attenuation_radius);;//perspective(spotlight.outer_angle, 1.0f, 0.1f, 1/spotlight.attenuation_radius);
		}
		mat4 operator()(const DirectionalLight& dirLight)
		{
			return ortho(-dirLight.width, dirLight.width, -dirLight.height, dirLight.height, 0.1f, 100.f);//perspective(spotlight.outer_angle, 1.0f, 0.1f, 1/spotlight.attenuation_radius);
		}
		template<typename T>
		mat4 operator()(T&) { return mat4{}; }
	};

	void Light::InitShadowMap()
	{
		std::visit([&](auto& light_variant) 
				{
					if(NeedShadowMap(light_variant))
						light_variant.InitShadowMap();
				}
		, light);
	}

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
					retval.falloff = (spotlight.use_inv_sq_atten) ? spotlight.attenuation_radius : (1 / (spotlight.attenuation_radius * spotlight.attenuation_radius));
					
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