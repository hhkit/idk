#include "stdafx.h"
#include "MonoEnvironment.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/threads.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/sgen-bridge.h>
#include <mono/utils/mono-dl-fallback.h>
#include <mono/metadata/mono-gc.h>
#include <mono/utils/mono-logger.h>

#include <script/ScriptSystem.h>

namespace idk::mono
{
	MonoEnvironment::MonoEnvironment(string_view path_to_game_dll)
	{
		char domain_name[] = "Script Domain";
		script_domain = mono_domain_create_appdomain(std::data(domain_name), 0);
		script_assembly = mono_domain_assembly_open(script_domain, path_to_game_dll.data());

		FindMonoBehaviors();
	}

	MonoEnvironment::~MonoEnvironment()
	{
		try
		{
			MonoObject* obj = nullptr;
			mono_domain_try_unload(script_domain, &obj); // try things
			IDK_ASSERT(obj == nullptr);
		}
		catch (...) {}

		script_domain = nullptr;
		script_assembly = nullptr;
	}

	void MonoEnvironment::FindMonoBehaviors()
	{
		auto script_image = mono_assembly_get_image(script_assembly);
		auto lib_image = mono_assembly_get_image(Core::GetSystem<ScriptSystem>().GetLibrary());

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
				auto& data = mono_behaviors[class_name];
				data.klass = class_ptr;
			}
		}
	}
}