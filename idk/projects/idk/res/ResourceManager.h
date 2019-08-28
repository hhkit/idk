#pragma once
#include <idk.h>
#include <idk_config.h>
#include <core/ISystem.h>
#include <res/ResourceFactory.h>
#include <res/ResourceHandle.h>
#include <res/ExtensionLoader.h>
#include <res/FileResources.h>
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
		void Shutdown() override;

		// loading ops
		void LoadDefaultResources();
		template<typename Factory, typename ... Args>
		Factory& RegisterFactory(Args&& ...);
		template<typename ExtensionLoader, typename ... Args>
		ExtensionLoader& RegisterExtensionLoader(std::string_view extension, Args&& ...);

		// handle ops
		template<typename Resource> bool      Validate(const RscHandle<Resource>&);
		template<typename Resource> Resource& Get(const RscHandle<Resource>&);
		template<typename Resource> bool      Free(const RscHandle<Resource>&);

		// resource creation
		template<typename Resource> RscHandle<Resource> Create();
		template<typename Resource> RscHandle<Resource> Create(string_view);
		template<typename Resource> RscHandle<Resource> Create(string_view path, Guid guid);
		template<typename Resource, 
			typename = sfinae<has_tag_v<Resource, MetaTag>>
		> RscHandle<Resource> Create(string_view path, Guid guid, const typename Resource::Meta& meta);

		// file operations
		FileResources LoadFile(std::string_view path_to_file);
		FileResources ReloadFile(std::string_view path_to_file);
		size_t        UnloadFile(std::string_view path_to_file);
		FileResources GetFileResources(std::string_view path_to_file);

	private:
		using GenPtr = shared_ptr<void>;
		template<typename R>
		using Storage = hash_table<Guid, std::shared_ptr<R>>;
		array<GenPtr, ResourceCount> _plaintext_loaders{}; // std::shared_ptr<ResourceFactory<Resource>>
		array<GenPtr, ResourceCount> _resource_tables  {}; // std::shared_ptr<Storage<Resource>>
		array<GenPtr, ResourceCount> _default_resources{}; // std::shared_ptr<Resource>

		hash_table<string, unique_ptr<ExtensionLoader>> _extension_loaders;
		hash_table<string, FileResources>               _loaded_files;

		static ResourceManager* instance;

		template<typename Resource> auto& GetTable();
		template<typename Resource> auto FindHandle(const RscHandle<Resource>&);

		template<typename T>
		friend struct detail::ResourceManager_detail;
	};
}
#include "ResourceManager.inl"