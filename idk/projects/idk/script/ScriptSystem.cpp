#include "stdafx.h"
#include "ScriptSystem.h"
#include <mono/metadata/assembly.h>
#include <mono/metadata/threads.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/sgen-bridge.h>
#include <mono/utils/mono-dl-fallback.h>
#include <mono/metadata/mono-gc.h>
#include <mono/utils/mono-logger.h>
#include <mono/jit/jit.h>

#include <script/MonoBehavior.h>

namespace idk
{
	void ScriptSystem::Init()
	{
		auto exe_dir = string{ Core::GetSystem<FileSystem>().GetExeDir() };
		mono_set_dirs(
			(exe_dir + "/mono/lib/").data(),
			(exe_dir + "/mono/etc/").data()
		);

		domain = mono_jit_init("Master Domain");

		lib_assembly = mono_domain_assembly_open(domain,
			(exe_dir + "/idk.dll").data());

		mono_jit_exec(domain, lib_assembly, 0, nullptr);

		//mono_trace_set_print_handler([](const char* string, mono_bool is_stdout) {
		//	LOG(string);
		//	});

		//mono_trace_set_log_handler([](const char* log_domain, const char* log_level, const char* message, mono_bool fatal, void*) {
		//	LOG(fmt::format("Domain: {}, Msg: {}", log_domain ? log_domain : "NULL", message));
		//	if (fatal)
		//		Log::Get().Flush();
		//	}, nullptr);
	}

	void ScriptSystem::Shutdown()
	{
	}
	void ScriptSystem::ScriptStart(span<MonoBehavior> behaviors)
	{
	}
	void ScriptSystem::ScriptUpdate(span<MonoBehavior> behaviors)
	{
	}
	void ScriptSystem::ScriptUpdateCoroutines(span<MonoBehavior> behaviors)
	{
		for (auto& elem : behaviors)
			elem.UpdateCoroutines();
	}
	void ScriptSystem::ScriptLateUpdate(span<MonoBehavior> behaviors)
	{
	}
	void ScriptSystem::ScriptFixedUpdate(span<MonoBehavior>behaviors)
	{
	}
}