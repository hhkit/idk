#include "stdafx.h"
#include "MonoBehaviorEnvironment.h"

#include <thread>

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

#include <core/GameObject.inl>
#include <scene/SceneManager.h>
#include <file/FileSystem.h>
#include <util/ioutils.h>

namespace idk::mono
{
	MonoBehaviorEnvironment::MonoBehaviorEnvironment(string_view full_path_to_game_dll)
		: MonoEnvironment{}
	{
		// setup
		char domain_name[] = "ScriptDomain";
		_domain = mono_domain_create_appdomain(std::data(domain_name), 0);

		// open file
		while (true)
		{
			std::ifstream file{ full_path_to_game_dll, std::ios::binary };
			if (!file.is_open())
			{
				std::this_thread::yield();
				continue;
			}
			assembly_data = binarify(file);
			break;
		}
		// load assembly
		mono_domain_set(_domain, true);
		MonoImageOpenStatus status;
		auto img = mono_image_open_from_data(assembly_data.data(), (uint32_t) assembly_data.size(), true, &status);
		mono_assembly_setrootdir((string{ Core::GetSystem<FileSystem>().GetExeDir() } +"/engine_data").data());
		_assembly = mono_assembly_load_from(img, full_path_to_game_dll.data(), &status);
		Core::GetSystem<ScriptSystem>().Environment().Image();

		ScanTypes();
	}
	MonoBehaviorEnvironment::~MonoBehaviorEnvironment()
	{
		MonoObject* obj = nullptr;
		if (!mono_domain_set(Core::GetSystem<ScriptSystem>().Environment().Domain(), true))
			LOG_CRASH("Failed to set domain.");
		mono_domain_try_unload(_domain, &obj); // try things
		//IDK_ASSERT(obj == nullptr);

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
	}
	void MonoBehaviorEnvironment::FindMonoBehaviors()
	{
		auto script_image = mono_assembly_get_image(_assembly);
		auto lib_image = Core::GetSystem<ScriptSystem>().Environment().Image();
		Core::GetSystem<ScriptSystem>().Environment().ScanTypes();

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

			LOG_TO(LogPool::MONO, "SEARCHING %s:%s @ %p", name_space, class_name, class_ptr);

			const auto is_monobehavior = [&]() {
				auto check_parent = class_ptr;

				do
				{
					
					LOG_TO(LogPool::MONO, "  CHECKING %s:%s @ %p", mono_class_get_namespace(check_parent), mono_class_get_name(check_parent), check_parent);
					auto my_img = mono_class_get_image(check_parent);
					if (mono_class_get_name(check_parent) == string_view{ "MonoBehavior" })
						return true;
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

				//LOG_TO(LogPool::MONO, string{ "Investigating " } +class_name);
				constexpr auto find_method = [](ManagedType& type, string_view fn_name, int param_count = 0)
				{
					auto res = type.CacheThunk(fn_name, param_count);
				};

				//type.CacheMessages();
				
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
				find_method(type, "PausedUpdate");
				find_method(type, "UpdateCoroutines");
				type.FindRPCs();
				

				//if (!Core::GetSystem<ScriptSystem>().Environment().IsAbstract(mono_class_get_type(type.Raw())))
					mono_behaviors.emplace(class_name, &type);
			}
		}

		LOG_CRASH_TO(LogPool::MONO, "hack");
		name_list.clear();
		for (auto& [name, type] : mono_behaviors)
			name_list.emplace_back(name.data());
	}
}