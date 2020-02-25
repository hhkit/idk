#include "stdafx.h"
#include "ShadowMapPool.h"

#include <gfx/Light.h>
#include <gfx/LightTypes.h>

namespace idk
{
	ShadowMapPool::~ShadowMapPool()
	{
		//I think this gets called too late, (after resource manager freed the shadows or smth.
		//for (auto& entries : _lightmaps)
		//{
		//	for (auto& entry : entries.entries)
		//	{
		//		for (auto& sm : entry.shadows)
		//			sm.DeleteShadowMap();
		//	}
		//}
	}
	void ShadowMapPool::Restart()
	{
		for (auto& entries : _lightmaps)
		{
			entries.reset();
		}
	}
	vector<Lightmap> ShadowMapPool::Subpool::Get(const vector<Lightmap>& light)
	{
		if (next == entries.size())
		{
			grow(light);
		}
		return entries[next++].shadows;
	}
	vector<Lightmap> ShadowMapPool::GetShadowMaps(const Light& light)
	{
		return _lightmaps[light.light.index()].Get(light.GetLightMap());
	}

	vector<Lightmap> ShadowMapPool::GetShadowMaps(size_t light_index,const vector<Lightmap>& to_dup)
	{
		return _lightmaps[light_index].Get(to_dup);
	}

	void ShadowMapPool::Subpool::grow(const vector<Lightmap>& shadows)
	{
		Entry entry{ vector<Lightmap>{shadows.size()} };
		size_t i = 0;
		for (auto& l : entry.shadows)
		{
			l.light_map = {};
			l.InitShadowMap(shadows[i].GetConfig());
			++i;
		}
		//auto copied_lightmaps = std::visit([](auto& copy) {
		//	copy.ReleaseShadowMap();
		//	auto res = copy.InitShadowMap(); 
		//	copy.ReleaseShadowMap();
		//	return res;
		//	}, copy);
		entries.emplace_back(std::move(entry));
	}

	void ShadowMapPool::Subpool::reset()
	{
		next = 0;
	}

}