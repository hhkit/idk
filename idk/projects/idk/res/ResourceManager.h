#pragma once
#include <idk.h>
#include <idk_config.h>
#include <reflect/reflect.h>
#include <core/ISystem.h>
#include <file/FileHandle.h>
#include <res/ResourceFactory.h>
#include <res/ResourceHandle.h>
#include <res/FileResources.h>
#include <res/ExtensionLoader.h>
#include <meta/meta.h>
#include <meta/tag.h>

namespace idk
{
	namespace detail
	{
		template<typename T> struct ResourceManager_detail;
	}

	class ResourceManager 
		: public ISystem
	{
	public:
		ResourceManager() = default;

		void Init() override;
		void LateInit() override;
		void Shutdown() override;
		void WatchDirectory();

		// loading ops
		template<typename Factory, typename ... Args>
		Factory& RegisterFactory(Args&& ...);
		template<typename ExtensionLoaderT, typename ... Args>
		ExtensionLoaderT& RegisterExtensionLoader(std::string_view extension, Args&& ...);
		template<typename Factory>
		Factory& GetFactory();

		// handle ops
		template<typename Resource> bool      Validate(const RscHandle<Resource>&);
		template<typename Resource> Resource& Get(const RscHandle<Resource>&);
		template<typename Resource> bool      Free(const RscHandle<Resource>&);

		// resource creation
		template<typename Resource> RscHandle<Resource> Create();
		template<typename Resource> RscHandle<Resource> Create(FileHandle);
		template<typename Resource> RscHandle<Resource> Create(FileHandle path, Guid guid);
		template<typename Resource, 
			typename = sfinae<has_tag_v<Resource, MetaTag>>
		> RscHandle<Resource> Create(FileHandle path, Guid guid, const typename Resource::Metadata& meta);

		template<typename RegisterMe, typename ... Args, 
			typename = sfinae<std::is_constructible_v<RegisterMe, Args...>>
		> RscHandle<RegisterMe> Create(Args&& ...);
		
		template<typename RegisterMe, typename ... Args, 
			typename = sfinae<std::is_constructible_v<RegisterMe, Args...>>
		> RscHandle<RegisterMe> Create(Guid guid, Args&& ...);

		// file operations
		FileResources LoadFile(FileHandle path_to_file);
		FileResources LoadFile(FileHandle path_to_file, const MetaFile& meta);
		FileResources ReloadFile(FileHandle path_to_file);
		size_t        UnloadFile(FileHandle path_to_file);
		FileResources GetFileResources(FileHandle path_to_file);

		// saving metadata
		void SaveDirtyMetadata();

	private:
		using GenPtr = shared_ptr<void>;
		template<typename R>
		using Storage = hash_table<Guid, std::shared_ptr<R>>;
		array<GenPtr, ResourceCount> _resource_factories{}; // std::shared_ptr<ResourceFactory<Resource>>
		array<GenPtr, ResourceCount> _resource_tables   {}; // std::shared_ptr<Storage<Resource>>
		array<GenPtr, ResourceCount> _default_resources {}; // std::shared_ptr<Resource>

		hash_table<string, unique_ptr<ExtensionLoader>> _extension_loaders;
		hash_table<string, FileResources>               _loaded_files;

		static ResourceManager* instance;

		template<typename Resource> auto& GetLoader();
		template<typename Resource> auto& GetTable();
		template<typename Resource> auto FindHandle(const RscHandle<Resource>&);

		template<typename T>
		friend struct detail::ResourceManager_detail;
	};
}
#include "ResourceManager.inl"