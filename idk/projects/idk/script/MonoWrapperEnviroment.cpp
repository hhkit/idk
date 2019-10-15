#include "stdafx.h"
#include "MonoWrapperEnviroment.h"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/threads.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/sgen-bridge.h>
#include <mono/utils/mono-dl-fallback.h>
#include <mono/metadata/mono-gc.h>
#include <mono/utils/mono-logger.h>

#include <IncludeComponents.h>
#include <IncludeResources.h>
#include <IncludeSystems.h>

#include <script/ValueBoxer.h>

namespace idk::mono
{
	MonoWrapperEnvironment::MonoWrapperEnvironment(string_view full_path_to_game_dll)
	{

		_domain = mono_jit_init("MasterDomain");
		_assembly = mono_domain_assembly_open(_domain, full_path_to_game_dll.data());

		BindCoreFunctions();

		IDK_ASSERT_MSG(_assembly, "cannot load idk.dll");

		{
			auto img = mono_assembly_get_image(_assembly);
			auto klass = mono_class_from_name(img, "idk", "IDK");
			auto method = mono_class_get_method_from_name(klass, "Main", 1);
			void* args[] = { 0 };
			mono_runtime_invoke(method, nullptr, args, nullptr);
		}

	}

	MonoWrapperEnvironment::~MonoWrapperEnvironment()
	{
		mono_jit_cleanup(_domain);
	}

	template<typename T>
	decltype(auto) decay(T&& arg)
	{
		return +arg;
	}

	void MonoWrapperEnvironment::BindCoreFunctions()
	{
		mono_add_internal_call("idk.Bindings::TransformGetPosition", decay([](Handle<Transform> id)
			{
				return id->GlobalPosition();
			}));
		
	}
}
