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
#include <res/FileLoader.h>
#include <meta/meta.h>
#include <meta/tag.h>

namespace idk
{
	namespace detail { template<typename T> struct ResourceManager_detail; }

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

	class ResourceManager 
		: public ISystem
	{
	public:
		template<typename Result, typename Err>
		using result = monadic::result<Result, Err>;

		ResourceManager() = default;

		void EmptyNewResources();
		void SaveDirtyFiles();
		void SaveDirtyMetadata();
		void WatchBuildDirectory();

		/* HANDLE CHECKING - related to handles */
		template<typename Res> bool Validate(const RscHandle<Res>&);
		template<typename Res> Res& Get     (const RscHandle<Res>&);
		template<typename Res> bool Free    (const RscHandle<Res>&);

		template<typename Res> opt<string_view> GetPath(const RscHandle<Res>&);
		bool									IsExtensionSupported(string_view ext);

		template<typename Res> [[nodiscard]] span<Res> SpanOverNew();

		/* RESOURCE LOADING - for high-level users like editor programmer */
		template<typename Res>  RscHandle<Res>        Create();
		
		/* FACTORIES - for registering resource factories */
		template<typename Factory>                    Factory& GetFactory();
		template<typename Factory, typename ... Args> Factory& RegisterFactory(string_view path, Args&& ... factory_construction_args);
	private:
		static inline ResourceManager* instance = nullptr;

		template<typename R> struct ResourceControlBlock;

		using Extension  = string;
		using GenericPtr = std::shared_ptr<void>;

		template<typename R> using ResourceStorage = hash_table<Guid, ResourceControlBlock<R>>;

		array<GenericPtr, ResourceCount> _default_resources; // std::shared_ptr<R>
		array<GenericPtr, ResourceCount> _resource_table;    // std::shared_ptr<hash_table<Guid, ResourceControlBlock<R>>>
		array<GenericPtr, ResourceCount> _factories;         // std::shared_ptr<ResourceFactory<R>>

		void Init()     override;
		void LateInit() override;
		void Shutdown() override;

		template<typename Res> auto& GetFactoryRes() { return *r_cast<ResourceFactory<Res>*>  (_factories        [BaseResourceID<Res>].get()); }
		template<typename Res> auto& GetTable()      { return *r_cast<ResourceStorage<Res>*>  (_resource_table   [BaseResourceID<Res>].get()); }
		template<typename Res> Res&  GetDefaultRes() { return *r_cast<Res*>                   (_default_resources[BaseResourceID<Res>].get()); }
		template<typename Res, typename = sfinae<has_tag_v<Res, Saveable>>> string GenUniqueName();
		template<typename Res> ResourceControlBlock<Res>* GetControlBlock(RscHandle<Res> handle);

		IFileLoader* GetLoader(string_view extension);

		template<typename T> friend struct detail::ResourceManager_detail;
	};

	template<typename R> 
	struct ResourceManager::ResourceControlBlock
	{
		bool          dirty    { false };
		opt<string>   path     { std::nullopt };
		shared_ptr<R> resource; // note: make atomic
		
		bool valid() const { return s_cast<bool>(resource); }
	};
}

#include "ResourceManager.inl"