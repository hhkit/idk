#include "ResourceManager.h"
#pragma once

namespace idk
{
	template<typename Factory, typename ...Args>
	Factory& ResourceManager::RegisterFactory(Args&& ... args)
	{
		auto ptr = std::make_shared<Factory>(std::forward<Args>(args)...);
		plaintext_loaders_[resource_ind<typename Factory::Resource>] = ptr;
		return *ptr;
	}
	template<typename Resource>
	inline bool ResourceManager::Validate(const RscHandle<Resource>& handle)
	{
		auto& table = *r_cast<Storage<Resource>*>(resource_tables_[resource_ind<Resource>].get());
		auto itr = table.find(handle.guid); 
		return itr != table.end();
	}
	template<typename Resource>
	inline Resource& ResourceManager::Get(const RscHandle<Resource>& handle)
	{
		auto& table = *r_cast<Storage<Resource>*>(resource_tables_[resource_ind<Resource>].get());
		if (auto itr = table.find(handle.guid); itr != table.end())
			return *itr->second;
		else
			return *r_cast<Resource*>(default_resources_[resource_ind<Resource>].get());
	}
}