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
#include <script/MonoEnvironment.h>
#include <script/MonoBehaviorEnvironment.h>
#include <script/MonoWrapperEnviroment.h>

namespace idk::mono
{
	void ScriptSystem::LoadGameScripts()
	{
		script_environment = std::make_unique<MonoBehaviorEnvironment>(Core::GetSystem<FileSystem>().GetFullPath(path_to_game_dll));
		script_environment->ScanTypes();
	}

	void ScriptSystem::UnloadGameScripts()
	{
		script_environment = nullptr;
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
		mono_trace_set_print_handler([](const char* string, mono_bool is_stdout) {
			LOG_TO(LogPool::GAME, string);
			});

		main_environment = std::make_unique<MonoWrapperEnvironment>(exe_dir + "/idk.dll");
		main_environment->ScanTypes();

		mono_trace_set_log_handler([](const char* log_domain, const char* log_level, const char* message, mono_bool fatal, void*) 
		{
			if (fatal)
			{
				LOG_TO(LogPool::FATAL, "Fatal Game Error");
				LOG_TO(LogPool::FATAL, message);
			}
			else
				LOG_TO(LogPool::GAME, message);
		}
		, nullptr);

		//LoadGameScripts();
	}

	void ScriptSystem::Shutdown()
	{
		//UnloadGameScripts();
		//main_environment = nullptr;
	}

	ScriptSystem::ScriptSystem() = default;
	ScriptSystem::~ScriptSystem() = default;

	void ScriptSystem::ScriptStart(span<Behavior> behaviors)
	{
	}
	void ScriptSystem::ScriptFixedUpdate(span<Behavior>behaviors)
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

	MonoEnvironment& ScriptSystem::Environment() const
	{
		return *main_environment;
	}

	MonoBehaviorEnvironment& ScriptSystem::ScriptEnvironment() const
	{
		return *script_environment;
	}

	void ScriptSystem::RefreshGameScripts()
	{
		//UnloadGameScripts();
		//LoadGameScripts();
	}
}