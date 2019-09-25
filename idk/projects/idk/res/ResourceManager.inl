#include "ResourceManager.h"
#include <res/ResourceMeta.h>
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
		auto itr = table.find(handle.guid);
		if (itr == table.end())
			return GetDefaultRes<Res>();
		else
		{
			auto& cb = itr->second;
			return cb.valid() ? GetDefaultRes<Res>() : *cb.resource;
		};
	}

	template<typename Res>
	inline bool ResourceManager::Free(const RscHandle<Res>& handle)
	{
		auto& table = GetTable<Res>();
		auto itr = table.find(handle.guid);
		if (itr == table.end())
			return false;
		else
		{
			table.erase(itr);
			return true;
		}
	}

	template<typename Res>
	inline string_view ResourceManager::GetPath(const RscHandle<Res>& h)
	{
		auto* cb = GetControlBlock(h);
		return cb ? cb->path : "";
	}

	template<typename Res>
	inline RscHandle<Res> ResourceManager::Create()
	{
		auto& factory = GetFactory<Res>();
		assert(&factory);

		auto& table = GetTable<Res>();
		auto [itr, success] = table.emplace(Guid::Make(), ResourceControlBlock<Res>{});

		auto& control_block = itr->second;
		// attempt to put on another thread
		{
			control_block.resource = factory.Create();
		}

		return RscHandle<Res>(itr->first);

	}

	template<typename Res>
	ResourceManager::CreateResult<Res> ResourceManager::Create(string_view path_to_new_asset)
	{
		auto adapted_path = string{ path_to_new_asset };
		{
			// make sure that file doesn't already exist
			auto itr = _loaded_files.find(adapted_path);
			if (itr != _loaded_files.end())
				return ResourceCreateError::PathAlreadyExists;
		}

		auto& factory = GetFactory<Res>();
		assert(&factory);

		auto& table = GetTable<Res>();
		auto [itr, success] = table.emplace(Guid::Make(), ResourceControlBlock{});

		auto& control_block = itr->second;
		control_block.path = path_to_new_asset;

		// attempt to put on another thread
		{
			control_block.resource = factory.Create();
		}

		return RscHandle<Res>(itr->first);
	}

	template<typename Res>
	ResourceManager::LoadResult<Res> ResourceManager::Load(PathHandle path)
	{
		auto res = Load(path);
		if (!res)
			return res.error();

		return res.value().Get<Res>();
	}

	template<typename Res>
	ResourceManager::SaveResult<Res> ResourceManager::Save(RscHandle<Res> result)
	{
		return SaveResult<Res>();
	}

	template<typename Res>
	ResourceReleaseResult ResourceManager::Release(RscHandle<Res> path)
	{
		auto& table = GetTable<Res>();
		auto itr = table.find(path.guid);
		if (itr == table.end())
			return ResourceReleaseResult::Error_ResourceNotLoaded;

		table.erase(itr);
		return ResourceReleaseResult::Ok;
	}

	template<typename Factory, typename ...Args>
	Factory& ResourceManager::RegisterFactory(Args&& ...factory_construction_args)
	{
		static_assert(has_tag_v<Factory, ResourceFactory>, "Can only register ResourceFactories");

		auto& ptr = _factories[BaseResourceID<typename Factory::Resource>] = std::make_shared<Factory>(std::forward<Args>(factory_construction_args)...);
		return *r_cast<Factory*>(ptr.get());
	}

	template<typename FLoader, typename ...Args>
	FLoader& ResourceManager::RegisterLoader(string_view ext, Args&& ...loader_construction_args)
	{
		static_assert(std::is_base_of_v<IFileLoader, FLoader>, "Can only register FileLoaders");

		auto& ptr = _file_loader[string{ ext }] = std::make_unique<FLoader>(std::forward<Args>(loader_construction_args)...);
		return *s_cast<FLoader*>(ptr.get());
	}

	template<typename Res, typename ...Args>
	RscHandle<Res> ResourceManager::LoaderEmplaceResource(Args&& ...construction_args)
	{
		return LoaderEmplaceResource<Res>(Guid::Make(), std::forward<Args>(construction_args)...);
	}

	template<typename Res, typename ...Args>
	inline RscHandle<Res> ResourceManager::LoaderEmplaceResource(Guid guid, Args&& ...construction_args)
	{
		auto& table = GetTable<Res>();
		auto& cb = table[guid]; // don't care just replace

		// attempt to put on other thread
		{
			cb.resource = std::make_unique<Res>(std::forward<Args>(construction_args)...);
		}

		return RscHandle<Res>{};
	}

	template<typename Res>
	ResourceManager::ResourceControlBlock<Res>* ResourceManager::GetControlBlock(RscHandle<Res> handle)
	{
		auto& table = GetTable<Res>();
		auto itr = table.find(handle.guid);
		return itr == table.end() ? nullptr : &itr->second;
	}
}