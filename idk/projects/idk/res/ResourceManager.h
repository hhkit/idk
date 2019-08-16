#pragma once
#include <idk.h>
#include <idk_config.h>
#include <core/ISystem.h>
#include <res/ResourceFactory.h>
#include <res/ResourceHandle.h>
#include <meta/meta.h>

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

		// resource creation
		template<typename Resource> RscHandle<Resource> Create(/* filesystem */);
		template<typename Resource> RscHandle<Resource> Create(string_view);
		//template<typename Resource> RscHandle<Resource> Create(/* filesystem, meta */);

		// handle ops
		template<typename Resource> bool                Validate(const RscHandle<Resource>&);
		template<typename Resource> Resource&           Get(const RscHandle<Resource>&);
	private:
		using GenPtr = shared_ptr<void>;
		template<typename R>
		using Storage = hash_table<Guid, std::shared_ptr<R>>;
		array<GenPtr, ResourceCount> plaintext_loaders_{};
		array<GenPtr, ResourceCount> resource_tables_  {};
		array<GenPtr, ResourceCount> default_resources_{};

		static ResourceManager* instance;

		template<typename T>
		friend struct detail::ResourceManager_detail;

		template<typename Resource>
		constexpr static auto resource_ind = index_in_tuple_v<Resource, Resources>;
	};
}
#include "ResourceManager.inl"