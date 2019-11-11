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
	MonoImage* MonoEnvironment::Image() const noexcept
	{
		return mono_assembly_get_image(_assembly);
	}
	MonoDomain* MonoEnvironment::Domain() const noexcept
	{
		return _domain;
	}
	const ManagedType* MonoEnvironment::Type(string_view name) const
	{
		auto itr = _types.find(string{ name });
		if (itr != _types.end())
			return &itr->second;

		return nullptr;
	}
	void MonoEnvironment::ScanTypes()
	{
		auto script_image = Image();

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

			_types.emplace(class_name, class_ptr);
		}
	}
}