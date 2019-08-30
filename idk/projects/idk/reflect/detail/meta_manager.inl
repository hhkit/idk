#pragma once

#include <reflect/reflect.h>

namespace idk::reflect::detail
{
	template<typename T, bool HasTypeDefinition>
	register_type<T, HasTypeDefinition>::register_type()
	{
		if constexpr (HasTypeDefinition)
		{
			static typed_context<T> context{};
			meta::instance().names_to_contexts.emplace(type_definition<T>::m_Name, &context);
			meta::instance().hashes_to_contexts.emplace(typehash<T>(), &context);
		}
		else
		{
			static typed_context_nodef<T> context{};
			meta::instance().names_to_contexts.emplace(context.name, &context);
			meta::instance().hashes_to_contexts.emplace(typehash<T>(), &context);
		}
	}
}