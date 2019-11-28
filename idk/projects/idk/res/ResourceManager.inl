#include "ResourceManager.h"
#include <res/ResourceMeta.h>
#include <res/SaveableResource.h>
#include <file/FileSystem.h>
#pragma once

namespace idk
{
	template<typename Res>
	bool ResourceManager::Validate(const RscHandle<Res>& handle)
	{
		return GetControlBlock(handle);
	}

	template<typename Res>
	inline Res& ResourceManager::Get(const RscHandle<Res>& handle)
	{
		auto& table = GetTable<Res>();
		const auto itr = table.find(handle.guid);
		if (itr == table.end())
			return GetDefaultRes<Res>();
		else
		{
			auto& cb = itr->second;
			return cb.valid() ? *cb.resource : GetDefaultRes<Res>();
		};
	}

	template<typename Res>
	inline vector<RscHandle<Res>> ResourceManager::GetAll()
	{
		auto& table = GetTable<Res>();
		auto retval = vector<RscHandle<Res>>{};
		retval.reserve(table.size());
		for (auto& [key, value] : table)
			retval.emplace_back(key);
		return retval;
	}

	template<typename Res>
	inline bool ResourceManager::Destroy(const RscHandle<Res>& handle)
	{
		auto& table = GetTable<Res>();
		const auto itr = table.find(handle.guid);
		if (itr == table.end())
			return false;
		else
		{
			table.erase(itr);
			return true;
		}
	}

	template<typename Res>
	inline opt<string_view> ResourceManager::GetPath(const RscHandle<Res>& h)
	{
		auto* cb = GetControlBlock(h);
		if (cb)
			if (cb->path)
				return *cb->path;
		return std::nullopt;
	}

	template<typename Res>
	RscHandle<Res> ResourceManager::Create()
	{
		return Create<Res>(Guid::Make());
	}

	template<typename Res>
	inline RscHandle<Res> ResourceManager::Create(Guid guid, bool replace)
	{
		auto& factory = GetFactoryRes<Res>();
		assert(&factory);
		return RscHandle<Res>{Emplace(guid, factory.Create(), replace)};
	}

	template<typename Res>
	inline RscHandle<Res> ResourceManager::Emplace(unique_ptr<Res> resource)
	{
		return Emplace(Guid::Make(), std::move(resource));
	}

	template<typename Res>
	inline RscHandle<Res> ResourceManager::Emplace(Guid guid, unique_ptr<Res> resource, bool replace)
	{
		auto& table = GetTable<Res>();
		auto [itr, success] = table.emplace(guid, ResourceControlBlock<Res>{});
		if (!success && !replace)
			return RscHandle<Res>();

		auto& control_block = itr->second;
		control_block.resource = std::move(resource);
		control_block.resource->_handle = RscHandle<typename Res::BaseResource>{ itr->first };
		return RscHandle<Res>{itr->first};
	}

	template<typename Res>
	inline RscHandle<Res> ResourceManager::Load(PathHandle h, bool reload)
	{
		auto& factory = GetFactoryRes<Res>();
		assert(&factory);
		return RscHandle<Res>{Emplace(Guid{ h.GetStem() }, factory.Create(h), reload)};
	}

	template<typename Factory>
	inline Factory& ResourceManager::GetFactory()
	{
		return *s_cast<Factory*>(_factories[BaseResourceID<typename Factory::Resource>].get());
	}

	template<typename Factory, typename ...Args>
	Factory& ResourceManager::RegisterFactory(Args&& ...factory_construction_args)
	{
		static_assert(has_tag_v<Factory, ResourceFactory>, "Can only register ResourceFactories");

		auto& ptr = _factories[BaseResourceID<typename Factory::Resource>] = std::make_shared<Factory>(std::forward<Args>(factory_construction_args)...);
		return *s_cast<Factory*>(ptr.get());
	}

	template<typename Res>
	Signal<RscHandle<Res>>& ResourceManager::OnResourceCreate()
	{
		return *s_cast<Signal<RscHandle<Res>>*>(_created_signals[BaseResourceID<Res>].get());
	}

	template<typename Res>
	Signal<RscHandle<Res>>& ResourceManager::OnResourceDestroy()
	{
		return *s_cast<Signal<RscHandle<Res>>*>(_destroying_signals[BaseResourceID<Res>].get());
	}

	template<typename Res>
	ResourceManager::ResourceControlBlock<Res>* ResourceManager::GetControlBlock(RscHandle<Res> handle)
	{
		auto& table = GetTable<Res>();
		const auto itr = table.find(handle.guid);
		return itr == table.end() ? nullptr : &itr->second;
	}
}