#include "stdafx.h"
#include "ResourceManager.h"
#include <IncludeResources.h>

namespace idk
{
	ResourceManager* ResourceManager::instance = nullptr;

	namespace detail
	{
		template<typename T> struct ResourceManager_detail;

		template<typename ... Rs>
		struct ResourceManager_detail<std::tuple<Rs...>>
		{
			static array<shared_ptr<void>, sizeof...(Rs)> GenResourceTables()
			{
				return array<shared_ptr<void>, sizeof...(Rs)>{
					std::make_shared<ResourceManager::Storage<Rs>>()...
				};
			}

			static array<shared_ptr<void>, sizeof...(Rs)> GenDefaults()
			{
				return array<shared_ptr<void>, sizeof...(Rs)>{
					std::make_shared<Rs>()...
				};
			}
		};

		using ResourceHelper = ResourceManager_detail<Resources>;
	}

	void ResourceManager::Init()
	{
		instance = this;
		resource_tables_   = detail::ResourceHelper::GenResourceTables();
		LoadDefaultResources();
	}

	void ResourceManager::Shutdown()
	{
		for (auto& elem : resource_tables_)
			elem.reset();

		for (auto& elem : default_resources_)
			elem.reset();
	}
	void ResourceManager::LoadDefaultResources()
	{
		default_resources_ = detail::ResourceHelper::GenDefaults();
	}
}