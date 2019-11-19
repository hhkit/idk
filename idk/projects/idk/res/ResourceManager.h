#pragma once
#include <idk.h>
#include <idk_config.h>
#include <ds/result.h>
#include <reflect/reflect.h>
#include <core/ISystem.h>
#include <event/Signal.h>
#include <file/PathHandle.h>
#include <res/ResourceBundle.h>
#include <res/ResourceFactory.h>
#include <res/ResourceHandle.h>
#include <res/ResourceUtils.h>
#include <meta/meta.h>
#include <meta/tag.h>

namespace idk
{
	namespace detail { template<typename T> struct ResourceManager_detail; }

	class ResourceManager 
		: public ISystem
	{
	public:
		template<typename Result, typename Err>
		using result = monadic::result<Result, Err>;

		/* LOOPS */
		void WatchBuildDirectory();

		/* HANDLE CHECKING - related to handles */
		template<typename Res> bool           Validate(const RscHandle<Res>&);
		template<typename Res> Res&           Get     (const RscHandle<Res>&);
		template<typename Res> RscHandle<Res> Create();
		template<typename Res> RscHandle<Res> Create(Guid guid);
		template<typename Res> RscHandle<Res> Load(PathHandle h, bool reload = true);
		template<typename Res> bool           Destroy(const RscHandle<Res>&);

		template<typename Res> opt<string_view> GetPath(const RscHandle<Res>&);
		bool									IsExtensionSupported(string_view ext);
		
		/* FACTORIES - for registering resource factories */
		template<typename Factory>                    Factory& GetFactory();
		template<typename Factory, typename ... Args> Factory& RegisterFactory(Args&& ... factory_construction_args);
		template<typename Res>                        void     RegisterExtension(string_view ext) { _extension_lut.emplace(ext, BaseResourceID<Res>); }

		template<typename Res> Signal<RscHandle<Res>>& OnResourceCreate();
		template<typename Res> Signal<RscHandle<Res>>& OnResourceDestroy();
	private:
		static inline ResourceManager* instance = nullptr;

		using Extension  = string;
		using GenericPtr = shared_ptr<void>;

		template<typename R> struct ResourceControlBlock;
		template<typename R> using  ResourceStorage = hash_table<Guid, ResourceControlBlock<R>>;

		hash_table<string_view, size_t>  _extension_lut; // maps extensions to resource IDs

		array<GenericPtr, ResourceCount> _default_resources;  // shared_ptr<R>
		array<GenericPtr, ResourceCount> _resource_table;     // shared_ptr<hash_table<Guid, ResourceControlBlock<R>>>
		array<GenericPtr, ResourceCount> _factories;          // shared_ptr<ResourceFactory<R>>

		array<GenericPtr, ResourceCount> _created_signals;    // shared_ptr<Signal<RscHandle<Res>>
		array<GenericPtr, ResourceCount> _destroying_signals; // shared_ptr<Signal<RscHandle<Res>>

		void Init()     override;
		void LateInit() override;
		void Shutdown() override;

		void LoadResource(PathHandle file);

		template<typename Res> auto& GetFactoryRes() { return *r_cast<ResourceFactory<Res>*>  (_factories        [BaseResourceID<Res>].get()); }
		template<typename Res> auto& GetTable()      { return *r_cast<ResourceStorage<Res>*>  (_resource_table   [BaseResourceID<Res>].get()); }
		template<typename Res> Res&  GetDefaultRes() { return *r_cast<Res*>                   (_default_resources[BaseResourceID<Res>].get()); }

		template<typename Res> ResourceControlBlock<Res>* GetControlBlock(RscHandle<Res> handle);

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