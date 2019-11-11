#pragma once

#include <idk.h>

namespace idk::reflect::detail
{
	struct meta
	{
		hash_table<string_view, struct typed_context_base*> names_to_contexts;
		hash_table<size_t, struct typed_context_base*> hashes_to_contexts;

		static meta& instance()
		{
			static meta s;
			return s;
		}
	};

	template<typename T, bool HasTypeDefinition>
	struct register_type
	{
		register_type();
	};
}