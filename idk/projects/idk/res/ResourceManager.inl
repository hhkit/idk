#include "ResourceManager.h"
#include <res/ResourceMeta.h>
#pragma once

namespace idk
{
	template<typename Factory, typename ...Args>
	Factory& ResourceManager::RegisterFactory(Args&& ... args)
	{
		auto ptr = std::make_shared<Factory>(std::forward<Args>(args)...);
		_plaintext_loaders[ResourceID<typename Factory::Resource>] = ptr;
		//_default_resources[ResourceID<typename Factory::Resource>] = ptr->Create();
		return *ptr;
	}
	template<typename ExtensionLoaderT, typename ... Args>
	inline ExtensionLoaderT& ResourceManager::RegisterExtensionLoader(std::string_view extension, Args&& ... args)
	{
		static_assert(std::is_base_of_v<ExtensionLoader, ExtensionLoaderT>, "can only register extension loaders");
		return *static_cast<ExtensionLoaderT*>((_extension_loaders[string{ extension }] = std::make_unique<ExtensionLoaderT>(std::forward<Args>(args)...)).get());
	}
	template<typename Resource>
	inline RscHandle<Resource> ResourceManager::Create()
	{
		auto& table = GetTable<Resource>();
		auto& loader = GetLoader<Resource>();
		if (&loader == nullptr)
			return RscHandle<Resource>{};
		auto ptr = loader.Create();
		if (!ptr)
			return RscHandle<Resource>{};
		
		auto handle = RscHandle<Resource>{ Guid::Make() };
		ptr->_handle = RscHandle<BaseResource_t<Resource>>{ handle };
		ptr->_dirty = true;
		if constexpr (has_tag_v<Resource, MetaTag>)
			ptr->_dirtymeta = true;
		table.emplace(handle.guid, std::move(ptr));
		
		return handle;
	}
	template<typename Resource>
	inline RscHandle<Resource> ResourceManager::Create(FileHandle filepath)
	{
		auto retval = Create<Resource>(filepath, Guid::Make());
		if (!retval)
			return RscHandle<Resource>{};
		retval->_dirty = true;
		if constexpr (has_tag_v<Resource, MetaTag>)
			retval->_dirtymeta = true;
		return retval;
	}

	template<typename Resource>
	inline RscHandle<Resource> ResourceManager::Create(FileHandle filepath, Guid guid)
	{
		auto [table, itr] = FindHandle(RscHandle<Resource>{guid});
		if (itr != table.end())
			return RscHandle<Resource>{};

		auto ptr = GetLoader<Resource>().Create(filepath);
		if (!ptr)
			return RscHandle<Resource>{};

		auto handle = RscHandle<Resource>{ guid };
		ptr->_handle = handle;
		table.emplace_hint(itr, handle.guid, std::move(ptr));
		return handle;
	}

	template<typename Resource, typename>
	inline RscHandle<Resource> ResourceManager::Create(FileHandle filepath, Guid guid, const typename Resource::Metadata& meta)
	{
		auto [table, itr] = FindHandle(RscHandle<Resource>{guid});
		if (itr != table.end())
			return RscHandle<Resource>{};

		auto ptr = GetLoader<Resource>().Create(filepath, meta);
		if (!ptr)
			return RscHandle<Resource>{};

		auto handle = RscHandle<Resource>{ guid };
		ptr->_handle = handle;
		table.emplace_hint(itr, handle.guid, std::move(ptr));
		return handle;
	}

	template<typename RegisterMe, typename ...Args, typename>
	RscHandle<RegisterMe> ResourceManager::Create(Args&& ... args)
	{
		auto& table = GetTable<RegisterMe>();
		auto& loader = GetLoader<RegisterMe>();
		if (&loader == nullptr)
			return RscHandle<RegisterMe>{};
		auto ptr = std::make_shared<RegisterMe>(std::forward<Args>(args)...);
		if (!ptr)
			return RscHandle<RegisterMe>{};

		auto handle = RscHandle<RegisterMe>{ Guid::Make() };
		ptr->_handle = RscHandle<BaseResource_t<RegisterMe>>{ handle };
		ptr->_dirty = true;
		if constexpr (has_tag_v<RegisterMe, MetaTag>)
			ptr->_dirtymeta = true;
		table.emplace(handle.guid, std::move(ptr));

		return handle;
	}

	template<typename RegisterMe, typename ...Args, typename>
	RscHandle<RegisterMe> ResourceManager::Create(Guid guid, Args&& ... args)
	{
		auto [table, itr] = FindHandle(RscHandle<RegisterMe>{guid});
		if (itr != table.end())
			return RscHandle<Resource>{};

		auto ptr = std::make_unique<RegisterMe>(std::forward<Args>(args)...);
		if (!ptr)
			return RscHandle<RegisterMe>{};

		auto handle = RscHandle<RegisterMe>{ guid };
		ptr->_handle = RscHandle<BaseResource_t<RegisterMe>>{ handle };
		table.emplace_hint(itr, handle.guid, std::move(ptr));
		return handle;
	}

	template<typename Resource>
	inline auto& ResourceManager::GetLoader()
	{
		return *r_cast<ResourceFactory<BaseResource_t<Resource>>*>(_plaintext_loaders[ResourceID<BaseResource_t<Resource>>].get());
	}

	template<typename Resource>
	inline auto& ResourceManager::GetTable()
	{
		return *r_cast<Storage<BaseResource_t<Resource>>*>(_resource_tables[ResourceID<BaseResource_t<Resource>>].get());
	}

	template<typename Resource>
	auto ResourceManager::FindHandle(const RscHandle<Resource>& handle)
	{
		auto& table = GetTable<BaseResource_t<Resource>>();
		return std::tuple<Storage<BaseResource_t<Resource>>&, Storage<BaseResource_t<Resource>>::iterator>{table, table.lower_bound(handle.guid)};
	}

	template<typename Resource>
	inline bool ResourceManager::Validate(const RscHandle<Resource>& handle)
	{
		auto [table, itr] = FindHandle(RscHandle<BaseResource_t<Resource>>{handle});
		return itr != table.end() && itr->second->_loaded;
	}

	template<typename Resource>
	inline Resource& ResourceManager::Get(const RscHandle<Resource>& handle)
	{
		auto [table, itr] = FindHandle(handle);
		if (itr != table.end() && itr->second->_loaded)
			return s_cast<Resource&>(*itr->second);
		else
			return *r_cast<Resource*>(_default_resources[ResourceID<BaseResource_t<Resource>>].get());
	}
	template<typename Resource>
	inline bool ResourceManager::Free(const RscHandle<Resource>& handle)
	{
		auto [table, itr] = FindHandle(handle);
		if (itr == table.end())
			return false;

		table.erase(itr);
		return true;
	}
}