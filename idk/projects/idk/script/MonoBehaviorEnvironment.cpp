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
#include <script/MonoBehavior.h>
#include <script/ValueUnboxer.h>

#include <core/GameObject.h>
#include <scene/SceneManager.h>

namespace idk::mono
{
	MonoBehaviorEnvironment::MonoBehaviorEnvironment(string_view full_path_to_game_dll)
		: MonoEnvironment{}
	{
		char domain_name[] = "ScriptDomain";
		_domain = mono_domain_create_appdomain(std::data(domain_name), 0);
		_assembly = mono_domain_assembly_open(_domain, full_path_to_game_dll.data());

		ScanTypes();
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
	void MonoBehaviorEnvironment::Init()
	{
		FindMonoBehaviors();
	}
	ManagedType* MonoBehaviorEnvironment::GetBehaviorMetadata(string_view name)
	{
		auto itr = mono_behaviors.find(string{ name });
		if (itr != mono_behaviors.end())
			return itr->second;
		else
			return nullptr;
	}
	span<const char* const> MonoBehaviorEnvironment::GetBehaviorList()
	{
		return span<const char* const>(name_list);
	}
	void MonoBehaviorEnvironment::Execute()
	{
		auto go = Core::GetSystem<SceneManager>().GetActiveScene()->CreateGameObject();
		auto tfm = go->Transform();

		auto& env = Core::GetSystem<ScriptSystem>().Environment();
		auto tfm_class = env.Type("Transform");
		auto tfm_obj = tfm_class->Construct();
		tfm_obj.Assign("handle", tfm.id);
		auto mb = go->AddComponent<mono::Behavior>();
		
		auto* obj = mb->EmplaceBehavior("Test");
		auto test = GetBehaviorMetadata("Test");

		//{
		//	auto update = test->GetMethod("Update");
		//
		//	void* args[] = { 0 };
		//	MonoObject* exception{};
		//	mono_runtime_invoke(std::get<MonoMethod*>(update), obj, args, &exception);
		//}
		//{
		//	auto thunderbolt = test->GetMethod("Thunderbolt", 1);
		//	vec3 v{ 2,3,4 };
		//	void* args[] = { &v, 0 };
		//	mono_runtime_invoke(std::get<MonoMethod*>(thunderbolt), obj, args, nullptr);
		//}
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
			thunk.Invoke(obj, tfm_obj);
		}
		{
			ManagedObject managed{ obj };
			
			//managed.VisitImpl([](auto&& key, auto&& val, int) { 
			//	using T = std::decay_t<decltype(val)>;
			//	if constexpr (std::is_same_v<T, int>)
			//		val += 5;
			//	
			//	std::cout << "YOLO:" << key << ":" << reflect::get_type<T>().name() << "\n"; 
			//	}, depth);
			auto thunk = std::get<ManagedThunk>(test->GetMethod("Thunderbolt", 1));
			thunk.Invoke(obj, vec3{ 8,9,10 });

			//std::cout << "SERIALIZED TEST:" << serialize_text(*mb);
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
				auto itr = _types.find(class_name);

				IDK_ASSERT(itr != _types.end());
				auto& type = itr->second;

				LOG_TO(LogPool::MONO, string{ "Investigating " } +class_name);
				constexpr auto find_method = [](ManagedType& type, string_view fn_name, int param_count = 0)
				{
					auto res = type.CacheThunk(fn_name, param_count);
					if (res)
						LOG_TO(LogPool::MONO, string{ "Found function " } +string{ fn_name });
				};

				find_method(type, "Clone");
				find_method(type, "Awake");
				find_method(type, "Start");
				find_method(type, "FixedUpdate");
				find_method(type, "OnTriggerEnter", 1);
				find_method(type, "OnTriggerStay", 1);
				find_method(type, "OnTriggerExit", 1);
				find_method(type, "OnCollisionEnter", 1);
				find_method(type, "OnCollisionStay", 1);
				find_method(type, "OnCollisionExit", 1);
				find_method(type, "Update");
				find_method(type, "UpdateCoroutines");


				//if (!Core::GetSystem<ScriptSystem>().Environment().IsAbstract(mono_class_get_type(type.Raw())))
					mono_behaviors.emplace(class_name, &type);
			}
		}

		name_list.clear();
		for (auto& [name, type] : mono_behaviors)
			name_list.emplace_back(name.data());
	}
}