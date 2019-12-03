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

#include <process.h>

namespace idk::mono
{
	void ScriptSystem::LoadGameScripts()
	{
		path_to_used_dll = string{ Core::GetSystem<FileSystem>().GetExeDir() } +"/" + std::filesystem::path{ GetConfig().path_to_game_dll }.stem().string() + ".dll";

		if(0)
		if (Core::GetSystem<FileSystem>().Exists(GetConfig().path_to_game_dll))
		{
			// recompile
			//auto stem = PathHandle{ GetConfig().path_to_game_dll }.GetStem();
			//auto path_to_sln = PathHandle{ string{"/scripts/"} + string{stem} + ".sln" };
			//_spawnl("")
			//system((string{ "\"" } +string{ PathHandle{"/tools/build.bat"}.GetFullPath() } +string{ "\"" }).data());
			//// copy dll to .exe location
			auto src_file = Core::GetSystem<FileSystem>().Open(GetConfig().path_to_game_dll, FS_PERMISSIONS::READ, true);
			std::ofstream dst_file;
			dst_file.open(path_to_used_dll, std::ios::binary | std::ios::out);
			dst_file << src_file.rdbuf();
			dst_file.flush();
		}
		else
			LOG_CRASH("Could not detect game dll!");
		
		if (Core::GetSystem<FileSystem>().Exists(GetConfig().path_to_game_dll))
		{
			script_environment = std::make_unique<MonoBehaviorEnvironment>(PathHandle{ GetConfig().path_to_game_dll }.GetFullPath());
			script_environment->Init();
		}
		else
			LOG_CRASH("Could not copy game dll!");
	}

	void ScriptSystem::UnloadGameScripts()
	{
		const auto max_gen = mono_gc_max_generation();
		for (int i = 0; i < max_gen; ++i)
			mono_gc_collect(i);
		script_environment = nullptr;
	}

	void ScriptSystem::Init()
	{
		auto exe_dir = string{ Core::GetSystem<FileSystem>().GetExeDir() };
		mono_set_dirs(
			(exe_dir + "/mono/lib/").data(),
			(exe_dir + "/mono/etc/").data()
		);
		mono_trace_set_print_handler([](const char* string, [[maybe_unused]] mono_bool is_stdout) {
			LOG_TO(LogPool::MONO, string);
			});

		mono_trace_set_log_handler([]([[maybe_unused]] const char* log_domain, [[maybe_unused]] const char* log_level, const char* message, mono_bool fatal, void*)
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
		if (Core::GetSystem<FileSystem>().ExistsFull(exe_dir + "/engine_data/idk.dll"))
		{
			main_environment = std::make_unique<MonoWrapperEnvironment>(exe_dir + "/engine_data/idk.dll");
			main_environment->Init();
		}
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

	void ScriptSystem::ApplyConfig([[maybe_unused]] Config& config)
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
			if (elem.enabled && elem.GetHandle().scene != Scene::prefab)
				elem.Start();
	}
	void ScriptSystem::ScriptFixedUpdate(span<Behavior>behaviors)
	{
		IDK_ASSERT(run_scripts);
		for (auto& elem : behaviors)
			if (elem.enabled && elem.GetHandle().scene != Scene::prefab)
				elem.FixedUpdate();
	}

	void ScriptSystem::ScriptUpdate(span<Behavior> behaviors)
	{
		IDK_ASSERT(run_scripts);
		for (auto& elem : behaviors)
			if (elem.enabled && elem.GetHandle().scene != Scene::prefab)
				elem.Update();
	}

	void ScriptSystem::ScriptPausedUpdate(span<Behavior> behaviors)
	{
		if (run_scripts)
			for (auto& elem : behaviors)
				if (elem.enabled && elem.GetHandle().scene != Scene::prefab)
					elem.FireMessage("PausedUpdate");
	}

	void ScriptSystem::ScriptUpdateCoroutines(span<Behavior> behaviors)
	{
		for (auto& elem : behaviors)
			if (elem.enabled && elem.GetHandle().scene != Scene::prefab)
				elem.UpdateCoroutines();
	}

	void ScriptSystem::ScriptLateUpdate([[maybe_unused]] span<Behavior> behaviors)
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