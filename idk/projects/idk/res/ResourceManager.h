#pragma once
#include <idk.h>
#include <idk_config.h>
#include <ds/result.h>
#include <reflect/reflect.h>
#include <core/ISystem.h>
#include <file/PathHandle.h>
#include <res/ResourceFactory.h>
#include <res/ResourceHandle.h>
#include <res/FileLoader.h>
#include <meta/meta.h>
#include <meta/tag.h>

namespace idk
{
	namespace detail { template<typename T> struct ResourceManager_detail; }

	enum class ResourceLoadError : char
	{
		NoFactoryRegistered,
		FileDoesNotExist,
		FailedToLoad
	};

	enum class ResourceSaveError : char
	{
		ResourceNotLoaded,
		ResourceNotReflected,
	};

	enum class ResourceReleaseError : char
	{
		ResourceNotLoaded,
	};

	class ResourceManager 
		: public ISystem
	{
	public:
		template<typename Result, typename Err>
		using result = monadic::result<Result, Err>;

		template<typename Res> using LoadResult = result<RscHandle<Res>, ResourceLoadError>;
		template<typename Res> using SaveResult = result<RscHandle<Res>, ResourceSaveError>;


		ResourceManager() = default;

		void Init() override;
		void LateInit() override;
		void Shutdown() override;
		void WatchDirectory();

		/* HANDLE CHECKING - related to handles */
		template<typename Res> bool Validate(const RscHandle<Res>&);
		template<typename Res> Res& Get     (const RscHandle<Res>&);
		template<typename Res> bool Free    (const RscHandle<Res>&);

		/* RESOURCE LOADING - for high-level users like editor programmer */
		template<typename Res> RscHandle<Res>       Create();
		template<typename Res> RscHandle<Res>       Create(std::string_view path_to_new_asset);
		template<typename Res> LoadResult<Res>      Load(PathHandle path);
		template<typename Res> SaveResult<Res>      Save(RscHandle<Res>);
		template<typename Res> ResourceReleaseError Release(RscHandle<Res>);

		/* FACTORIES - for registering resource factories */
		template<typename Factory, typename ... Args> Factory& RegisterFactory(Args&& ... factory_construction_args);
		template<typename Factory, typename ... Args> Factory& RegisterLoader (Args&& ... loader_construction_args);
		/* FACTORY RESOURCE LOADING - FACTORIES SHOULD CALL THESE */
		template<typename Res, typename ... Args> [[nodiscard]] RscHandle<Res> LoaderEmplaceResource(Args&& ... construction_args); 
		/* THIS WILL REPLACE EXISTING RESOURCE IF IT EXISTS */
		template<typename Res, typename ... Args> [[nodiscard]] RscHandle<Res> LoaderEmplaceResource(Guid, Args&& ... construction_args);

	private:
		using Extension  = std::string;
		using GenericPtr = std::shared_ptr<void>;
		template<typename R>
		struct ResourceControlBlock
		{
			bool dirty;
			shared_ptr<R> resource; // note: make atomic
		};

		array<GenericPtr, ResourceCount> _resources;                   // std::shared_ptr<ResourceControlBlock<R>>
		array<GenericPtr, ResourceCount> _factories;                   // std::shared_ptr<ResourceFactory<R>>
		hash_table<Extension, unique_ptr<IFileLoader>> _file_loader;   // extension to file loader
	};

	template<typename Res>
	RscHandle<Res> ResourceManager::Create()
	{
		return RscHandle<Res>();
	}
}
#include "ResourceManager.inl"