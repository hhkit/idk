#include "stdafx.h"
#include "Light.h"
#include <core/GameObject.inl>
#include <common/Transform.h>
#include <math/matrix_transforms.inl>
#include <math/angle.inl>

#include<iostream>
std::ostream& operator<<(std::ostream& out, const idk::vec3& v)
{
	return out << "{" << v.x << ", " << v.y << ", " << v.z << " }";
}
namespace idk
{
	struct LightCameraView
	{
		const Light* light;
		mat4 operator()(const PointLight&)
		{
			return translate(-light->GetGameObject()->Transform()->GlobalPosition());
		}
		mat4 operator()(const SpotLight& )
		{
			const auto trf = light->GetGameObject()->Transform();
			const vec3 v_pos = trf->GlobalPosition();
			const vec3 v_dir = trf->Forward();
			return look_at(v_pos, v_pos + v_dir, trf->Up()).inverse();
		}
		mat4 LookAt(vec3 pos, vec3 target_point,vec3 up)
		{
			return look_at(pos, target_point, up);
		}
		mat4 View(const mat4& tfm)
		{
			auto mat =tfm;
			auto retval = orthonormalize(mat);
			retval[3] = mat[3];

			return retval.inverse();
		}
		mat4 operator()(const DirectionalLight&)
		{
			auto trf = light->GetGameObject()->Transform();
			const vec3 v_pos = vec3(0, 0, 0);
			const vec3 v_dir = trf->Forward();
			return look_at(v_pos, v_pos - vec3(v_dir.x, v_dir.y, v_dir.z), trf->Up()).inverse();
		}

		/*vector<mat4> operator()(const PointLight&)
		{
			auto trf = light->GetGameObject()->Transform();
			vec3 v_pos = trf->GlobalPosition();
			vec3 v_dir = trf->Forward();

			vector<mat4> matList;
			matList.emplace_back(look_at(v_pos, v_pos - vec3(1.f, 0.f, 0.f), vec3(0.f, -1.f, 0.f)).inverse());
			matList.emplace_back(look_at(v_pos, v_pos - vec3(-1.f, 0.f, 0.f), vec3(0.f, -1.f, 0.f)).inverse());
			matList.emplace_back(look_at(v_pos, v_pos - vec3(0.f, 1.f, 0.f), vec3(0.f, 0.f, 1.f)).inverse());
			matList.emplace_back(look_at(v_pos, v_pos - vec3(0.f, -1.f, 0.f), vec3(0.f, 0.f, -1.f)).inverse());
			matList.emplace_back(look_at(v_pos, v_pos - vec3(0.f, 0.f, 1.f), vec3(0.f, -1.f, 0.f)).inverse());
			matList.emplace_back(look_at(v_pos, v_pos - vec3(0.f, 0.f, -1.f), vec3(0.f, -1.f, 0.f)).inverse());

			return matList;
		}*/

		template<typename T>
		mat4 operator()(T&) { return mat4{}; }

		//template<typename T>
		//vector<mat4> operator()(T&) { return vector<mat4>{}; }
	};
	struct LightCameraProj
	{
		mat4 operator()([[maybe_unused]] PointLight pointlight)
		{
			return perspective(deg{ 90 }, 1.0f, 0.1f, 100.f);//perspective(spotlight.outer_angle, 1.0f, 0.1f, 1/spotlight.attenuation_radius);
		}
		mat4 operator()(SpotLight spotlight)
		{
			return perspective(spotlight.outer_angle*2, 1.0f, 0.1f, (spotlight.use_inv_sq_atten) ? (1 / spotlight.attenuation_radius) : spotlight.attenuation_radius);//perspective(spotlight.outer_angle, 1.0f, 0.1f, 1/spotlight.attenuation_radius);
		}
		mat4 operator()(const DirectionalLight& dirLight)
		{
			return ortho(dirLight.left, dirLight.right, dirLight.bottom, dirLight.top, dirLight.near, dirLight.far);
		}
		template<typename T>
		mat4 operator()(T&) { return mat4{}; }
	};
	Light::~Light()
	{
		if (!_copied)
		{
			std::visit([](auto& light) {
				light.DeleteShadowMap(); 
				}, light);
		}
	}

	bool Light::is_active_and_enabled() const
	{
		return enabled && GetGameObject()->ActiveInHierarchy();
	}

	Uncopied::Uncopied(const Uncopied&) :_is_copied{ true } {}
	Uncopied::Uncopied(Uncopied&& rhs) noexcept :_is_copied{ rhs._is_copied } { rhs._is_copied = true; }

	Uncopied& Uncopied::operator=(Uncopied&& rhs) noexcept { std::swap(_is_copied, rhs._is_copied); return*this; }
	bool Uncopied::copied() const noexcept { return _is_copied; }
	void Uncopied::copied(bool val) noexcept { _is_copied = val; }
	Uncopied& Uncopied::operator=(const Uncopied&) noexcept { copied(true); return *this; }
	Uncopied& Uncopied::operator=(bool val)noexcept { _is_copied = val; return *this; }

	void Light::InitShadowMap()
	{
		bool is_copied = _copied;
		std::visit([&](auto& light_variant) 
				{
					if(is_copied||NeedShadowMap(light_variant))
					{
						light_variant.ReleaseShadowMap();
						light_variant.InitShadowMap();
					}
				}
		, light);
		_copied = false;
	}

	vector<Lightmap>& Light::GetLightMap()
	{
		return
			std::visit([&](auto& light_variant) ->vector<Lightmap> &
				{
					return light_variant.GetShadowMap();
				}
		, light);
	}
	const vector<Lightmap>& Light::GetLightMap() const
	{
		return
			std::visit([&](auto& light_variant)-> const vector<Lightmap> &
				{
					return light_variant.GetShadowMap();
				}
		, light);
	}
	void Light::SetLightMap(const vector<Lightmap>& light_map)
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
					const auto tfm = GetGameObject()->Transform();
					retval.v_pos = tfm->GlobalPosition();
					retval.v_dir = vec3(0.f);
					retval.intensity = point_light.intensity;
					retval.cos_inner = 1;
					retval.cos_outer = 1;
					retval.falloff = (point_light.use_inv_sq_atten) ? (point_light.attenuation_radius * point_light.attenuation_radius): point_light.attenuation_radius;
					//vp = ortho() * look_at(retval.v_pos, retval.v_pos + retval.v_dir, vec3{ 0,1,0 });
				}

				if constexpr (std::is_same_v<T, DirectionalLight>)
				{
					const DirectionalLight& dir_light = light_variant;
					retval.light_color = dir_light.light_color * dir_light.intensity;
					const auto tfm = GetGameObject()->Transform();
					retval.v_pos = tfm->GlobalPosition();
					retval.v_dir = tfm->Forward();
					retval.intensity = dir_light.intensity;
					retval.cos_inner = 0;
					retval.cos_outer = 1.f;
					retval.falloff = 1.f;
				}

				if constexpr (std::is_same_v<T, SpotLight>)
				{
					const SpotLight& spotlight = light_variant;
					retval.light_color = spotlight.light_color * spotlight.intensity;
					const auto tfm = GetGameObject()->Transform();
					retval.v_pos = tfm->GlobalPosition();
					retval.v_dir = tfm->Forward();
					retval.cos_inner = cos(spotlight.inner_angle);
					retval.cos_outer = cos(spotlight.outer_angle);
					retval.falloff = (spotlight.use_inv_sq_atten) ? (1.f / (spotlight.attenuation_radius * spotlight.attenuation_radius)) : spotlight.attenuation_radius;
					
					retval.intensity = spotlight.intensity;
					//vp = :spotlight.attenuation_radius)*look_at(retval.v_pos, retval.v_pos + retval.v_dir, vec3{ 0,1,0 });
				}
				retval.shadow_layers = shadow_layers;
				retval.update_shadow = update_shadow;
				retval.shadow_bias = shadow_bias;
				retval.v = LightCameraView{ this }(light_variant);
				retval.p = LightCameraProj{}(light_variant);
				retval.vp = retval.p * retval.v;
				//retval.w = GetGameObject()->Transform()->GlobalMatrix();
				retval.light_maps = light_variant.GetShadowMap();
				retval.cast_shadow = casts_shadows;
			}
		, light);

		return retval;
	}

	

	//LightData Light::GenerateLightData() const
	//{
	//	LightData retval;
	//	retval.index = (int)light.index();
	//	std::visit([&](auto& light_variant)
	//	{
	//		using T = std::decay_t<decltype(light_variant)>;
	//		if constexpr (std::is_same_v<T, PointLight>)
	//		{
	//			const PointLight& point_light = light_variant;
	//			retval.light_color = point_light.light_color * point_light.intensity;
	//			retval.v_pos = GetGameObject()->Transform()->GlobalPosition();
	//			retval.intensity = point_light.intensity;
	//			//vp = ortho() * look_at(retval.v_pos, retval.v_pos + retval.v_dir, vec3{ 0,1,0 });
	//		}

	//		if constexpr (std::is_same_v<T, DirectionalLight>)
	//		{
	//			const DirectionalLight& dir_light = light_variant;
	//			retval.light_color = dir_light.light_color * dir_light.intensity;
	//			const auto tfm = GetGameObject()->Transform();
	//			retval.v_pos = tfm->GlobalPosition();
	//			retval.v_dir = tfm->Forward();
	//			retval.intensity = dir_light.intensity;
	//		}

	//		if constexpr (std::is_same_v<T, SpotLight>)
	//		{
	//			const SpotLight& spotlight = light_variant;
	//			retval.light_color = spotlight.light_color * spotlight.intensity;
	//			const auto tfm = GetGameObject()->Transform();
	//			retval.v_pos = tfm->GlobalPosition();
	//			retval.v_dir = tfm->Forward();
	//			retval.cos_inner = cos(spotlight.inner_angle);
	//			retval.cos_outer = cos(spotlight.outer_angle);
	//			retval.falloff = (spotlight.use_inv_sq_atten) ? spotlight.attenuation_radius : (1 / (spotlight.attenuation_radius * spotlight.attenuation_radius));

	//			retval.intensity = spotlight.intensity;
	//			//vp = :spotlight.attenuation_radius)*look_at(retval.v_pos, retval.v_pos + retval.v_dir, vec3{ 0,1,0 });
	//		}
	//		retval.shadow_bias = shadow_bias;
	//		retval.v = LightCameraView{ this }(light_variant);
	//		retval.p = LightCameraProj{}(light_variant);
	//		retval.vp = retval.p * retval.v;
	//		retval.light_map = light_variant.light_map;
	//		retval.cast_shadow = casts_shadows;
	//	}
	//	, light);

	//	return retval;
	//}

	void Light::SetLightData(const LightData& ld)
	{
		std::visit([&,ld](auto& light_variant)
		{
			using T = std::decay_t<decltype(light_variant)>;
			if constexpr (std::is_same_v<T, PointLight>)
			{
				//const PointLight& point_light = light_variant;
				light_variant.light_color = ld.light_color;
				light_variant.intensity = ld.intensity;
				//vp = ortho() * look_at(retval.v_pos, retval.v_pos + retval.v_dir, vec3{ 0,1,0 });
			}

			if constexpr (std::is_same_v<T, DirectionalLight>)
			{
				//const DirectionalLight& dir_light = light_variant;
				light_variant.light_color = ld.light_color;
				light_variant.intensity = ld.intensity;
			}

			if constexpr (std::is_same_v<T, SpotLight>)
			{
				//const SpotLight& spotlight = light_variant;
				light_variant.light_color = ld.light_color;
				light_variant.intensity = ld.intensity;
				//vp = :spotlight.attenuation_radius)*look_at(retval.v_pos, retval.v_pos + retval.v_dir, vec3{ 0,1,0 });
			}
		}
		, light);
	}
	color Light::GetColor() const
	{
		return std::visit([&](auto& light_variant)
		{
			return light_variant.light_color;
		}
		, light);
	}
	void Light::SetColor(const color& c)
	{
		std::visit([&](auto& light_variant)
		{
			light_variant.light_color = c;
		}
		, light);
	}
	real Light::GetLightIntensity() const
	{
		return std::visit([&](auto& light_variant)
		{
			return light_variant.intensity;
		}
		, light);
	}
	void Light::SetLightIntensity(const real& i)
	{
		std::visit([&](auto& light_variant)
		{
			light_variant.intensity = i;
		}
		, light);
	}
}