#include "stdafx.h"
#include "ScriptSystem.h"

#include <filesystem>

#include <mono/metadata/assembly.h>
#include <mono/metadata/threads.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/sgen-bridge.h>
#include <mono/utils/mono-dl-fallback.h>
#include <mono/metadata/mono-gc.h>
#include <mono/utils/mono-logger.h>
#include <mono/jit/jit.h>

#include <proj/ProjectManager.h>

#include <script/MonoBehavior.h>
#include <script/MonoEnvironment.h>
#include <script/MonoBehaviorEnvironment.h>
#include <script/MonoWrapperEnvironment.h>

namespace idk::mono
{
	void ScriptSystem::LoadGameScripts()
	{
		path_to_used_dll = string{ Core::GetSystem<FileSystem>().GetExeDir() } +"/" + std::filesystem::path{ GetConfig().path_to_game_dll }.stem().string() + ".dll";

		if (Core::GetSystem<FileSystem>().Exists(GetConfig().path_to_game_dll))
		{
			// copy dll to .exe location
			auto src_file = Core::GetSystem<FileSystem>().Open(GetConfig().path_to_game_dll, FS_PERMISSIONS::READ, true);
			std::ofstream dst_file;
			dst_file.open(path_to_used_dll, std::ios::binary | std::ios::out);
			dst_file << src_file.rdbuf();
		}
		else
			LOG_CRASH("Could not detect game dll!");

		if (Core::GetSystem<FileSystem>().ExistsFull(path_to_used_dll))
		{
			script_environment = std::make_unique<MonoBehaviorEnvironment>(path_to_used_dll);
			script_environment->Init();
		}
		else
			LOG_CRASH("Could not copy game dll!");
	}

	void ScriptSystem::UnloadGameScripts()
	{
		script_environment = nullptr;
	}

	void ScriptSystem::Init()
	{
		auto exe_dir = string{ Core::GetSystem<FileSystem>().GetExeDir() };
		mono_set_dirs(
			(exe_dir + "/mono/lib/").data(),
			(exe_dir + "/mono/etc/").data()
		);
		mono_trace_set_print_handler([](const char* string, mono_bool is_stdout) {
			LOG_TO(LogPool::MONO, string);
			});

		if (Core::GetSystem<FileSystem>().ExistsFull(exe_dir + "/idk.dll"))
		{
			main_environment = std::make_unique<MonoWrapperEnvironment>(exe_dir + "/idk.dll");
			main_environment->Init();
		}
		mono_trace_set_log_handler([](const char* log_domain, const char* log_level, const char* message, mono_bool fatal, void*)
			{
				if (fatal)
				{
					LOG_TO(LogPool::MONO, "Fatal Game Error");
					LOG_TO(LogPool::MONO, message);
				}
				else
					LOG_TO(LogPool::MONO, message);
			}
		, nullptr);
	}

	void ScriptSystem::LateInit()
	{
		Core::GetSystem<FileSystem>().Mount(
			string{ Core::GetSystem<ProjectManager>().GetProjectDir() } + "/Scripts",
			"/scripts",
			true);

		LoadGameScripts();
	}

	void ScriptSystem::Shutdown()
	{
		UnloadGameScripts();
		main_environment = nullptr;
	}

	void ScriptSystem::ApplyConfig(Config& config)
	{
	}

	ScriptSystem::ScriptSystem() = default;
	ScriptSystem::ScriptSystem(const ScriptSystem&)
	{
		IDK_ASSERT(false);
	}
	ScriptSystem::~ScriptSystem() = default;

	void ScriptSystem::ScriptStart(span<Behavior> behaviors)
	{
		for (auto& elem : behaviors)
			elem.Awake();

		for (auto& elem : behaviors)
			if (elem.enabled)
				elem.Start();
	}
	void ScriptSystem::ScriptFixedUpdate(span<Behavior>behaviors)
	{
		for (auto& elem : behaviors)
			if (elem.enabled)
				elem.FixedUpdate();
	}

	void ScriptSystem::ScriptUpdate(span<Behavior> behaviors)
	{
		for (auto& elem : behaviors)
			if (elem.enabled)
				elem.Update();
	}

	void ScriptSystem::ScriptUpdateCoroutines(span<Behavior> behaviors)
	{
		for (auto& elem : behaviors)
			elem.UpdateCoroutines();
	}

	void ScriptSystem::ScriptLateUpdate(span<Behavior> behaviors)
	{
	}

	MonoWrapperEnvironment& ScriptSystem::Environment() const
	{
		return *main_environment;
	}

	MonoBehaviorEnvironment& ScriptSystem::ScriptEnvironment() const
	{
		return *script_environment;
	}

	void ScriptSystem::RefreshGameScripts()
	{
		UnloadGameScripts();
		LoadGameScripts();
	}
}