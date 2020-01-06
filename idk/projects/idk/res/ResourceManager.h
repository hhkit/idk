#pragma once
#include <idk.h>
#include <idk_config.h>
#include <ds/result.h>
#include <reflect/reflect.h>
#include <core/ISystem.h>
#include <file/PathHandle.h>
#include <res/ResourceBundle.h>
#include <res/ResourceFactory.h>
#include <res/ResourceHandle.h>
#include <res/ResourceUtils.h>
#include <res/IAssetLoader.h>
#include <res/FileLoader.h>
#include <meta/meta.h>
#include <meta/tag.h>

namespace idk
{
	namespace detail { 
		template<typename T> struct ResourceManager_detail;
		template<typename T> struct CompiledAssetHelper_detail;
	}

	enum class ResourceCreateError : char
	{
		PathAlreadyExists,
	};

	enum class ResourceLoadError : char
	{
		ExtensionNotRegistered,
		FileDoesNotExist,
		FailedToLoadResource,
	};

	enum class BundleGetError : char
	{
		ResourceNotLoaded
	};


	enum class ResourceSaveError : char
	{
		ResourceFailedToSave,
		ResourceNotLoaded,
		ResourceNotReflected,
		TargetFilePathAlreadyExists,
	};

	enum class ResourceReleaseResult : char
	{
		Ok,
		Error_ResourceNotLoaded,
	};

	enum class FileMoveResult : char
	{
		Ok,
		Error_ResourceNotFound,
		Error_DestinationExists
	};

	template<typename T, typename Meta> struct MetaResource;

	class ResourceManager 
		: public ISystem
	{
	public:
		template<typename Result, typename Err>
		using result = monadic::result<Result, Err>;

		using GeneralLoadResult = result<ResourceBundle, ResourceLoadError>; // todo: return resource bundle by const T&
		using GeneralGetResult  = result<ResourceBundle, BundleGetError>; // todo: return resource bundle by const T&
		template<typename Res> using CreateResult = result<RscHandle<Res>, ResourceCreateError>;
		template<typename Res> using GetResult   = result<RscHandle<Res>,  BundleGetError>;
		template<typename Res> using LoadResult   = result<RscHandle<Res>, ResourceLoadError>;
		template<typename Res> using SaveResult   = result<RscHandle<Res>, ResourceSaveError>;

		ResourceManager() = default;

		static ResourceManager& Instance() noexcept;
		void EmptyNewResources();
		void SaveDirtyFiles();
		void SaveDirtyMetadata();
		void WatchDirectory();

		/* HANDLE CHECKING - related to handles */
		template<typename Res> bool Validate(const RscHandle<Res>&);
		template<typename Res> Res& Get     (const RscHandle<Res>&);
		template<typename Res> bool Free    (const RscHandle<Res>&);

		template<typename Res> opt<string_view> GetPath(const RscHandle<Res>&);
		bool									IsExtensionSupported(string_view ext);

		template<typename Res> [[nodiscard]] span<Res> SpanOverNew();

		/* RESOURCE LOADING - for high-level users like editor programmer */
		template<typename Res>  RscHandle<Res>        Create  ();
		template<typename Res>  CreateResult<Res>     Create  (string_view path_to_new_asset);
		template<typename Res>  LoadResult<Res>       Load    (PathHandle path, bool reload_resource = true);
		                        GeneralLoadResult     Load    (PathHandle path, bool reload_resource = true);
								void                  LoadAsync(PathHandle path, bool wait = false);
								void                  LoadCompiledAsset(PathHandle path);
								void                  Unload  (PathHandle path);
		template<typename Res>  GetResult<Res>        Get     (PathHandle path);
								GeneralGetResult      Get     (PathHandle path);
		template<typename Res>  vector<RscHandle<Res>>GetAll();
		template<typename Res>  SaveResult<Res>       Save    (RscHandle<Res> result);
		template<typename Res>  SaveResult<Res>       CopyTo  (RscHandle<Res> result, string_view new_mountpath);
		template<typename Res>  SaveResult<Res>       MoveTo(string_view curr_mountpath, string_view new_mountpath);
		template<typename Res>  ResourceReleaseResult Release (RscHandle<Res>);
		                        FileMoveResult        Rename(PathHandle old_path, string_view new_mount_path);
		template<typename Res>  FileMoveResult        Rename(RscHandle<Res> resource, string_view new__mountpath);
		
		/* FACTORIES - for registering resource factories */
		template<typename Factory>                    Factory& GetFactory();
		template<typename Factory, typename ... Args> Factory& RegisterFactory(Args&& ... factory_construction_args);
		template<typename FLoader, typename ... Args> FLoader& RegisterLoader (string_view ext, Args&& ... loader_construction_args);
		template<typename ALoader>                    void RegisterAssetLoader();
		                                              void RegisterCompilableExtension(string_view ext);
		/* FACTORY RESOURCE LOADING - FACTORIES SHOULD CALL THESE */
		template<typename Res>                        [[nodiscard]] RscHandle<Res> LoaderCreateResource(Guid);
		template<typename Res,     typename ... Args> [[nodiscard]] RscHandle<Res> LoaderEmplaceResource(Args&& ... construction_args); 
		template<typename Res,     typename ... Args> [[nodiscard]] RscHandle<Res> LoaderEmplaceResource(Guid, Args&& ... construction_args);

	private:
		static inline ResourceManager* instance = nullptr;

		template<typename R> struct ResourceControlBlock;
		struct FileControlBlock;

		using Extension  = string;
		using Path       = string;
		using GenericPtr = std::shared_ptr<void>;
		template<typename R>
		using ResourceStorage = hash_table<Guid, ResourceControlBlock<R>>;

		array<GenericPtr, ResourceCount>               _default_resources; // std::shared_ptr<R>
		array<GenericPtr, ResourceCount>               _resource_table;    // std::shared_ptr<hash_table<Guid, ResourceControlBlock<R>>>
		array<GenericPtr, ResourceCount>               _new_resources;     // std::vector<RscHandle<R>>
		array<GenericPtr, ResourceCount>               _factories;         // std::shared_ptr<ResourceFactory<R>>
		hash_table<string_view, shared_ptr<IAssetLoader>> _compiled_asset_loader; // std::shared_ptr<IAssetLoader<R>>
		hash_table<Extension, unique_ptr<IFileLoader>> _file_loader;
		hash_table<Path,      FileControlBlock>        _loaded_files;
		hash_set<string> _compilable_extensions;

		void Init()     override;
		void LateInit() override;
		void Shutdown() override;

		template<typename Res> auto& GetFactoryRes() { return *r_cast<ResourceFactory<Res>*>  (        _factories[BaseResourceID<Res>].get()); }
		template<typename Res> auto& GetTable()      { return *r_cast<ResourceStorage<Res>*>  (   _resource_table[BaseResourceID<Res>].get()); }
		template<typename Res> Res&  GetDefaultRes() { return *r_cast<Res*>                   (_default_resources[BaseResourceID<Res>].get()); }
		template<typename Res> auto& GetNewVector () { return *r_cast<vector<RscHandle<Res>>*>(    _new_resources[BaseResourceID<Res>].get()); }
		template<typename Res, typename = sfinae<has_tag_v<Res, Saveable>>> string GenUniqueName();
		template<typename Res> ResourceControlBlock<Res>* GetControlBlock(RscHandle<Res> handle);

		IFileLoader* GetLoader(string_view extension);
		MetaBundle GetMeta(PathHandle path_to_file);

		template<typename T> friend struct detail::ResourceManager_detail;
		template<typename T> friend struct detail::CompiledAssetHelper_detail;
		template<typename T, typename Meta> friend struct MetaResource;
	};

	template<typename R> 
	struct ResourceManager::ResourceControlBlock
	{
		bool             dirty    { false };
		opt<string>      path     { std::nullopt };
		shared_ptr<R>    resource; // note: make atomic

		// meta data
		shared_ptr<void> userdata; 
		bool             dirty_meta { false };
		
		bool valid() const { return s_cast<bool>(resource); }
	};

	struct ResourceManager::FileControlBlock
	{
		ResourceBundle bundle;
		bool is_new { false };
	};
}
