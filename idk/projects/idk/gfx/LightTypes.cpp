#include "stdafx.h"
#include "LightTypes.h"
#include <gfx/RenderTarget.h>
#include <gfx/FramebufferFactory.h>
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>
//#include "LightTypes.h"

namespace idk
{
	constexpr uvec2 shadow_map_dim{ 512,512};

	const void* PointLight::unique_id() const noexcept
	{
		static const char test[] = "PointLight";
		return s_cast<const char*>(test);
	}
	vector<Lightmap>& SpotLight::GetShadowMap()
	{
		// TODO: insert return statement here
		return light_map;
	}
	const vector<Lightmap>& SpotLight::GetShadowMap() const
	{
		// TODO: insert return statement here
		return light_map;
	}
	const void* SpotLight::unique_id() const noexcept
	{
		static const char test[] = "SpotLight";
		return s_cast<const char*>(test);
	}
	vector<Lightmap>& DirectionalLight::GetShadowMap()
	{
		return light_map;
	}
	const vector<Lightmap>& DirectionalLight::GetShadowMap() const
	{
		// TODO: insert return statement here
		return light_map;
	}
	const void* DirectionalLight::unique_id() const noexcept
	{
		static const char test[] = "DirectionalLight";
		return s_cast<const char*>(test);
	}
	template<typename T>
	bool NeedShadowMapImpl(T& light)
	{
		return !!light.light_map.empty();
	}
	bool NeedShadowMap(const PointLight&light){return NeedShadowMapImpl(light);};
	bool NeedShadowMap(const SpotLight&light){return NeedShadowMapImpl(light);};
	bool NeedShadowMap(const DirectionalLight&light){return NeedShadowMapImpl(light);};
#pragma optimize("",off)
	vector<Lightmap> PointLight::InitShadowMap()
	{
		light_map.resize(1);
		//vector<Lightmap> framebuffers;
		for (auto& elem : light_map)
		{
			//elem.SetCascade(camData, cascadeiter[i++], cascadeiter[i]);

			//if (elem.NeedLightMap())
			auto handle= elem.InitShadowMap();
			auto& tmp = *handle;
		}
		return light_map;
	}
	void PointLight::DeleteShadowMap() noexcept
	{
		for (auto& lmp : light_map)
		{
			lmp.DeleteShadowMap();
		}
	}
	void PointLight::ReleaseShadowMap() noexcept
	{
		light_map.clear();
	}
	void SpotLight::ReleaseShadowMap() noexcept
	{
		light_map.clear();
	}
	void DirectionalLight::ReleaseShadowMap() noexcept
	{
		light_map.clear();
	}
	vector<Lightmap>& PointLight::GetShadowMap()
	{
		// TODO: insert return statement here
		return light_map;
	}
	const vector<Lightmap>& PointLight::GetShadowMap() const
	{
		// TODO: insert return statement here
		return light_map;
	}
	vector <Lightmap> DirectionalLight::InitShadowMap()
	{
		light_map.resize(cascade_count);
		for (auto& elem : light_map)
		{
			//elem.SetCascade(camData, cascadeiter[i++], cascadeiter[i]);

			//if (elem.NeedLightMap())
			elem.InitShadowMap();
		}
		return light_map;
	}

	vector<Lightmap> DirectionalLight::InitShadowMap() const
	{
		vector<Lightmap> lm{};
		lm.resize(cascade_count);
		for (auto& elem : lm)
		{
			//elem.SetCascade(camData, cascadeiter[i++], cascadeiter[i]);

			//if (elem.NeedLightMap())
			elem.InitShadowMap();
		}
		return lm;
	}
	void DirectionalLight::DeleteShadowMap() noexcept
	{
		for (auto& lmp : light_map)
		{
			lmp.DeleteShadowMap();
		}
	}
	vector < Lightmap> SpotLight::InitShadowMap()
	{
		light_map.resize(1);
		for (auto& elem : light_map)
		{
			//elem.SetCascade(camData, cascadeiter[i++], cascadeiter[i]);

			//if (elem.NeedLightMap())
			elem.InitShadowMap();
		}
		return light_map;
	}
	void SpotLight::DeleteShadowMap() noexcept
	{
		for (auto& lmp : light_map)
		{
			lmp.DeleteShadowMap();
		}
	}
}

