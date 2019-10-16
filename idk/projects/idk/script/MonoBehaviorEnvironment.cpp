#include "stdafx.h"
#include "MonoBehaviorEnvironment.h"

#include <iostream>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/threads.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/sgen-bridge.h>
#include <mono/utils/mono-dl-fallback.h>
#include <mono/metadata/mono-gc.h>
#include <mono/utils/mono-logger.h>

#include <script/ManagedObj.h>
#include <script/ScriptSystem.h>

#include <core/GameObject.h>

namespace idk::mono
{
	MonoBehaviorEnvironment::MonoBehaviorEnvironment(string_view full_path_to_game_dll)
		: MonoEnvironment{}
	{
		char domain_name[] = "ScriptDomain";
		_domain = mono_domain_create_appdomain(std::data(domain_name), 0);
		_assembly = mono_domain_assembly_open(_domain, full_path_to_game_dll.data());

		ScanTypes();
		FindMonoBehaviors();
	}
	MonoBehaviorEnvironment::~MonoBehaviorEnvironment()
	{
		try
		{
			MonoObject* obj = nullptr;
			mono_domain_try_unload(_domain, &obj); // try things
			IDK_ASSERT(obj == nullptr);
		}
		catch (...) {}

		_domain = nullptr;
		_assembly = nullptr;
	}
	opt<ManagedType> MonoBehaviorEnvironment::GetBehaviorMetadata(string_view name)
	{
		auto itr = mono_behaviors.find(string{ name });
		if (itr != mono_behaviors.end())
			return itr->second;
		else
			return std::nullopt;
	}
	void MonoBehaviorEnvironment::Execute()
	{
		auto test = GetBehaviorMetadata("Test");
		auto obj = test->Construct();
		{
			auto update = test->GetMethod("Update");

			void* args[] = { 0 };
			MonoObject* exception{};
			auto fetch = obj.Fetch();
			mono_runtime_invoke(std::get<MonoMethod*>(update), fetch, args, &exception);
		}
		{
			auto thunderbolt = test->GetMethod("Thunderbolt", 1);
			vec3 v{ 2,3,4 };
			void* args[] = { &v, 0 };
			mono_runtime_invoke(std::get<MonoMethod*>(thunderbolt), obj.Fetch(), args, nullptr);
		}
		{
			test->CacheThunk("Update", 0);
			auto thunk = std::get<ManagedThunk>(test->GetMethod("Update", 0));
			thunk.Invoke(obj);
		}
		{
			IDK_ASSERT(test->CacheThunk("Thunderbolt", 1));
			auto thunk = std::get<ManagedThunk>(test->GetMethod("Thunderbolt", 1));
			thunk.Invoke(obj, vec3{ 8,9,10 });
		}
		{
			IDK_ASSERT(test->CacheThunk("TestTransform", 1));
			auto thunk = std::get<ManagedThunk>(test->GetMethod("TestTransform", 1));
			std::cout << "invoke";
			auto go = Core::GetSystem<SceneManager>().GetActiveScene()->CreateGameObject();
			auto tfm = go->Transform();

			auto& env = Core::GetSystem<ScriptSystem>().Environment();
			auto tfm_class = env.Type("Transform");
			auto tfm_obj = tfm_class->Construct();
			tfm_obj.Assign("handle", tfm.id);

			//auto method = tfm_class->GetMethod("YoloVan", 1);
			//
			//void* args[] = { tfm_obj.Fetch() };
			//mono_runtime_invoke(std::get<MonoMethod*>(method), tfm_obj.Fetch(), args, nullptr);
			thunk.Invoke(obj, tfm_obj);
		}
	}
	void MonoBehaviorEnvironment::FindMonoBehaviors()
	{
		auto script_image = mono_assembly_get_image(_assembly);
		auto lib_image = Core::GetSystem<ScriptSystem>().Environment().Image();

		auto monobehavior = mono_class_from_name(lib_image, "idk", "MonoBehavior");

		auto table = mono_image_get_table_info(script_image, MONO_TABLE_TYPEDEF);
		const auto rows = mono_table_info_get_rows(table);

		for (int i = 0; i < rows; ++i)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(table, i, std::data(cols), MONO_TYPEDEF_SIZE);

			auto class_name = mono_metadata_string_heap(script_image, cols[MONO_TYPEDEF_NAME]);
			auto name_space = mono_metadata_string_heap(script_image, cols[MONO_TYPEDEF_NAMESPACE]);

			auto class_ptr = mono_class_from_name(script_image, name_space, class_name);

			if (class_name == std::string("<Module>") || class_ptr == nullptr)
				continue;

			const auto is_monobehavior = [&]() {
				auto check_parent = class_ptr;

				do
				{
					if (check_parent == monobehavior)
						return true;
					check_parent = mono_class_get_parent(check_parent);
				} while (check_parent);
				return false;
			}();

			if (is_monobehavior)
			{
				mono_behaviors.emplace(class_name, ManagedType{ class_ptr });
			}
		}
	}
}