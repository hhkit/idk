#include "stdafx.h"
#include "ScriptSystem.h"

#include <iostream>

#include <mono/metadata/assembly.h>
#include <mono/metadata/threads.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/sgen-bridge.h>
#include <mono/utils/mono-dl-fallback.h>
#include <mono/metadata/mono-gc.h>
#include <mono/utils/mono-logger.h>
#include <mono/jit/jit.h>

#include <script/MonoBehavior.h>
#include <script/MonoEnvironment.h>

namespace idk::mono
{
	void ScriptSystem::LoadGameScripts()
	{
		environment = std::make_unique<MonoEnvironment>(Core::GetSystem<FileSystem>().GetFullPath(path_to_game_dll));
	}

	void ScriptSystem::Init()
	{
		Core::GetSystem<FileSystem>().Mount(
			string{ Core::GetSystem<FileSystem>().GetExeDir() } + "/scripts",
			"/scripts", 
			true);

		auto exe_dir = string{ Core::GetSystem<FileSystem>().GetExeDir() };
		mono_set_dirs(
			(exe_dir + "/mono/lib/").data(),
			(exe_dir + "/mono/etc/").data()
		);

		domain = mono_jit_init("Master Domain");

		lib_assembly = mono_domain_assembly_open(domain,
			(exe_dir + "/idk.dll").data()); 
		IDK_ASSERT_MSG(lib_assembly, "cannot load idk.dll");


		mono_trace_set_print_handler([](const char* string, mono_bool is_stdout) {
			std::cout << string;
			});

		//mono_trace_set_log_handler([](const char* log_domain, const char* log_level, const char* message, mono_bool fatal, void*) {
		//	LOG(fmt::format("Domain: {}, Msg: {}", log_domain ? log_domain : "NULL", message));
		//	if (fatal)
		//		Log::Get().Flush();
		//	}, nullptr);

		mono_jit_exec(domain, lib_assembly, 0, nullptr);
		// LoadGameScripts();
	}

	void ScriptSystem::Shutdown()
	{
	}

	ScriptSystem::ScriptSystem() = default;
	ScriptSystem::~ScriptSystem() = default;

	void ScriptSystem::ScriptStart(span<Behavior> behaviors)
	{
	}
	void ScriptSystem::ScriptUpdate(span<Behavior> behaviors)
	{
	}
	void ScriptSystem::ScriptUpdateCoroutines(span<Behavior> behaviors)
	{
		for (auto& elem : behaviors)
			elem.UpdateCoroutines();
	}
	void ScriptSystem::ScriptLateUpdate(span<Behavior> behaviors)
	{
	}
	MonoAssembly* ScriptSystem::GetLibrary() const
	{
		return lib_assembly;
	}
	void ScriptSystem::ScriptFixedUpdate(span<Behavior>behaviors)
	{
	}
}