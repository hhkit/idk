#include "stdafx.h"
#include "ManagedType.h"

#include <script/ScriptSystem.h>
#include <script/MonoWrapperEnvironment.h>

namespace idk::mono
{
	ManagedType::ManagedType(MonoClass* type)
		: type{type}
	{
	}

	MonoClass* ManagedType::Raw() const
	{
		return type;
	}

	bool ManagedType::CacheThunk(string_view method_name, int param_count)
	{
		auto method = FindMethod(method_name, param_count);

		if (!method)
			return false;

		thunks.emplace(string{ method_name }, ManagedThunk{ method });
		return true;
	}
	std::variant<ManagedThunk, MonoMethod*, std::nullopt_t> ManagedType::GetMethod(string_view method_name, int param_count) const
	{
		{
			auto itr = thunks.find(string{ method_name });
			if (itr != thunks.end())
				return itr->second;
		}
		
		{
			auto method = FindMethod(method_name, param_count);
			if (method)
				return method;
		}

		return std::nullopt;
	}
	MonoMethod* ManagedType::FindMethod(string_view method_name, int param_count) const
	{
		auto find_class = type;
		auto obj_type = Core::GetSystem<ScriptSystem>().Environment().Type("Object");
		IDK_ASSERT(obj_type);
		auto obj = obj_type->Raw();
		while (find_class != obj)
		{
			auto retval = mono_class_get_method_from_name(find_class, method_name.data(), param_count);
			if (retval)
				return retval;
			find_class = mono_class_get_parent(find_class);
		}

		return mono_class_get_method_from_name(find_class, method_name.data(), param_count);
		//return nullptr;
	}
}