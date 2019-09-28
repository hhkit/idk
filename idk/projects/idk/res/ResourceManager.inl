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
			return cb.valid() ? *cb.resource : GetDefaultRes<Res>();
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
		if (cb)
		{
			if (cb->path)
				return *cb->path;
		}
		return "";
//		return cb && cb->path ? *cb->path : "";
	}

	template<typename Res>
	inline RscHandle<Res> ResourceManager::Create()
	{
		auto& factory = GetFactoryRes<Res>();
		assert(&factory);

		auto& table = GetTable<Res>();
		auto [itr, success] = table.emplace(Guid::Make(), ResourceControlBlock<Res>{});

		auto& control_block = itr->second;
		// attempt to put on another thread
		{
			control_block.resource = factory.Create();
			control_block.resource->_handle = RscHandle<typename Res::BaseResource>{itr->first};
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

		auto& factory = GetFactoryRes<Res>();
		assert(&factory);

		auto& table = GetTable<Res>();
		auto [itr, success] = table.emplace(Guid::Make(), ResourceControlBlock<Res>{});

		auto& control_block = itr->second;
		control_block.path = adapted_path;

		// attempt to put on another thread
		{
			control_block.resource = factory.Create();
			control_block.resource->_handle = RscHandle<typename Res::BaseResource>{ itr->first };
		}

		auto& fcb = _loaded_files[adapted_path];
		fcb.is_new = true;
		fcb.bundle.Add(RscHandle<Res>(itr->first));

		return RscHandle<Res>(itr->first);
	}

	template<typename Res>
	ResourceManager::LoadResult<Res> ResourceManager::Load(PathHandle path, bool reload_resource)
	{
		auto res = Load(path, reload_resource);
		if (!res)
			return res.error();

		return res.value().Get<Res>();
	}
	
	template<typename Res>
	ResourceManager::GetResult<Res> ResourceManager::Get(PathHandle path)
	{
		auto bundle = Get(path);
		if (!bundle)
			return bundle.error();

		return bundle->Get<Res>();
	}

	template<typename Res>
	ResourceManager::SaveResult<Res> ResourceManager::Save(RscHandle<Res> saveme)
	{
		auto* rcb = GetControlBlock(saveme);

		if (!rcb)
			return ResourceSaveError::ResourceNotLoaded;
		
		auto filepath = [&]() -> string
		{
			if (rcb->path)
				return *rcb->path;
			else
				return GenUniqueName<Res>(); // gen unique name
		}();

		auto stream = Core::template GetSystem<FileSystem>().Open( filepath, FS_PERMISSIONS::WRITE);
		stream << serialize_text(*saveme);

		return saveme;
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

	template<typename Res>
	inline FileMoveResult ResourceManager::Rename(RscHandle<Res> resource, string_view new_path)
	{
		assert(false);
		auto* cb = GetControlBlock(resource);
		if (!cb)
			return FileMoveResult::Error_ResourceNotFound;

		if (cb->path)
			return Rename(PathHandle{ cb->path }, new_path);

		if (Core::template GetSystem<FileSystem>().Exists(new_path))
			return FileMoveResult::Error_DestinationExists;

		_loaded_files.emplace(string{ new_path }, ResourceBundle{ resource });

		return FileMoveResult::Ok;
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

	template<typename FLoader, typename ...Args>
	FLoader& ResourceManager::RegisterLoader(string_view ext, Args&& ...loader_construction_args)
	{
		static_assert(std::is_base_of_v<IFileLoader, FLoader>, "Can only register FileLoaders");

		auto& ptr = _file_loader[string{ ext }] = std::make_unique<FLoader>(std::forward<Args>(loader_construction_args)...);
		return *s_cast<FLoader*>(ptr.get());
	}

	template<typename Res>
	inline RscHandle<Res> ResourceManager::LoaderCreateResource(Guid guid)
	{
		auto& factory = GetFactoryRes<Res>();
		assert(&factory);

		auto& table = GetTable<Res>();
		auto [itr, success] = table.emplace(guid, ResourceControlBlock<Res>{});
		if (!success)
			return {};

		auto& control_block = itr->second;
		// attempt to put on another thread
		{
			control_block.resource = factory.Create();
			control_block.resource->_handle = RscHandle<Res>{ itr->first };
		}

		return RscHandle<Res>(itr->first);
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
			cb.resource->_handle = RscHandle<typename Res::BaseResource>{ guid };
		}

		return RscHandle<Res>{guid};
	}

	template<typename Res, typename>
	inline string ResourceManager::GenUniqueName()
	{
		auto start_name = string{ reflect::get_type<Res>().name() };

		auto make_path = [&]()
		{
			return "/assets/" + start_name + string{ Res::ext };
		};

		auto path = make_path();

		while (_loaded_files.find(path) != _loaded_files.end())
		{
			start_name += " (copy)";
			path = make_path();
		}

		return path;
	}

	template<typename Res>
	ResourceManager::ResourceControlBlock<Res>* ResourceManager::GetControlBlock(RscHandle<Res> handle)
	{
		auto& table = GetTable<Res>();
		auto itr = table.find(handle.guid);
		return itr == table.end() ? nullptr : &itr->second;
	}
}