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
	vector<Lightmap> PointLight::InitShadowMap()
	{
		light_map.resize(1);
		//vector<Lightmap> framebuffers;
		for (auto& elem : light_map)
		{
			//elem.SetCascade(camData, cascadeiter[i++], cascadeiter[i]);

			//if (elem.NeedLightMap())
			//elem.InitShadowMap(6, AttachmentViewType::eCube);//
			elem.InitShadowMap(1, AttachmentViewType::e2D);//Temp
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
//#pragma optimize("",off)
	vector <Lightmap> DirectionalLight::InitShadowMap()
	{
		light_map.resize(cascade_count);
		for (auto& elem : light_map)
		{
			//elem.SetCascade(camData, cascadeiter[i++], cascadeiter[i]);

			//if (elem.NeedLightMap())
			elem.InitShadowMap(cascade_count,AttachmentViewType::e2DArray);
		}
		return light_map;
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
			elem.InitShadowMap(1,AttachmentViewType::e2D);
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

