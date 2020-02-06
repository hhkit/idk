#include "stdafx.h"
#include "ShadowMapPool.h"

#include <gfx/Light.h>
#include <gfx/LightTypes.h>

namespace idk
{
	ShadowMapPool::~ShadowMapPool()
	{
		for (auto& entries : _lightmaps)
		{
			for (auto& entry : entries.entries)
			{
				for (auto& sm : entry.shadows)
					sm.DeleteShadowMap();
			}
		}
	}
	void ShadowMapPool::Restart()
	{
		for (auto& entries : _lightmaps)
		{
			entries.reset();
		}
	}
	vector<Lightmap> ShadowMapPool::Subpool::Get(const Light& light)
	{
		if (next == entries.size())
		{
			grow(light);
		}
		return entries[next++].shadows;
	}
	vector<Lightmap> ShadowMapPool::GetShadowMaps(const Light& light)
	{
		return _lightmaps[light.light.index()].Get(light);
	}

	void ShadowMapPool::Subpool::grow(const Light& light)
	{
		auto copy = light.light;
		auto copied_lightmaps = std::visit([](auto& copy) {
			copy.ReleaseShadowMap();
			auto res = copy.InitShadowMap(); 
			copy.ReleaseShadowMap();
			return res;
			}, copy);
		entries.emplace_back(Entry{ copied_lightmaps });
	}

	void ShadowMapPool::Subpool::reset()
	{
		next = 0;
	}

}