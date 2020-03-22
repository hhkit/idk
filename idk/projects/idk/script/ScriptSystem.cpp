#include "stdafx.h"
#include "ScriptSystem.h"

#include <file/FileSystem.h>

#include <mono/metadata/assembly.h>
#include <mono/metadata/threads.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/sgen-bridge.h>
#include <mono/utils/mono-dl-fallback.h>
#include <mono/metadata/mono-gc.h>
#include <mono/utils/mono-logger.h>
#include <mono/jit/jit.h>

#include <app/Application.h>
#include <proj/ProjectManager.h>

#include <editor/IEditor.h>
#include <script/MonoBehavior.h>
#include <script/MonoEnvironment.h>
#include <script/MonoBehaviorEnvironment.h>
#include <script/MonoWrapperEnvironment.h>

#include <process.h>
#include <ds/span.inl>
#include <sstream>

namespace idk::mono
{
	void ScriptSystem::LoadGameScripts()
	{
		path_to_used_dll = string{ Core::GetSystem<FileSystem>().GetExeDir() } + "/" +
            string(std::filesystem::path{ GetConfig().path_to_game_dll.sv() }.stem().string()) + ".dll";

		if (Core::GetSystem<FileSystem>().Exists(GetConfig().path_to_game_dll))
		{
			script_environment = std::make_unique<MonoBehaviorEnvironment>(string{ Core::GetSystem<ProjectManager>().GetProjectDir() } 
				+ string{ GetConfig().path_to_game_dll });
			script_environment->Init();
		}
		else
			LOG_CRASH("Could not copy game dll!");

	}

	void ScriptSystem::UnloadGameScripts()
	{
        if (script_environment)
        {
            const auto max_gen = mono_gc_max_generation();
            for (int i = 0; i < max_gen; ++i)
                mono_gc_collect(i);
            script_environment = nullptr;
        }
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
		if (&Core::GetSystem<IEditor>())
			CompileGameScripts();
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
		GameState::GetGameState().FlushCreationQueue();
		for (auto& elem : behaviors)
			elem.Awake();

		for (auto& elem : behaviors)
			if (elem.enabled && elem.GetHandle().scene != Scene::prefab)
				elem.Start();
	}
	void ScriptSystem::ScriptFixedUpdate(span<Behavior>behaviors)
	{
		IDK_ASSERT(run_scripts);
		GameState::GetGameState().FlushCreationQueue();
		for (auto& elem : behaviors)
			if (elem.enabled && elem.GetHandle().scene != Scene::prefab)
				elem.FixedUpdate();
	}

	void ScriptSystem::ScriptUpdate(span<Behavior> behaviors)
	{
		IDK_ASSERT(run_scripts);
		GameState::GetGameState().FlushCreationQueue();
		for (auto& elem : behaviors)
			if (elem.enabled && elem.GetHandle().scene != Scene::prefab)
				elem.Update();
	}

	void ScriptSystem::ScriptPausedUpdate(span<Behavior> behaviors)
	{
		GameState::GetGameState().FlushCreationQueue();
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

	void ScriptSystem::HandleException(MonoObject* exc) const
	{
		auto idk = Environment().Type("IDK");
		auto method = std::get<1>(idk->GetMethod("PrintException", 1));
		void* args[] = { exc, 0 };
		mono_runtime_invoke(method, nullptr, args, nullptr);
	}

	void ScriptSystem::RefreshGameScripts()
	{
		UnloadGameScripts();
		LoadGameScripts();
	}
	void ScriptSystem::CompileGameScripts()
	{
		auto proj_man = Core::GetSystem<ProjectManager>();
		auto path_to_sln = string{ proj_man.GetProjectDir() } +'/' + string{ proj_man.GetProjectName() } +".sln";

		auto wrap_string = [](string_view str) -> string
		{
			return "\"" + string{ str } +"\"";
		};

		vector<string> vals{ 
			"/C",
			wrap_string(
				wrap_string(string{ Core::GetSystem<FileSystem>().GetExeDir() } +"/tools/" + "build.bat") + " " +
				wrap_string(path_to_sln) + " " +
				string{proj_man.GetProjectName()} + " " +
				"Release" + " " +
				wrap_string(string{ Core::GetSystem<FileSystem>().GetExeDir() } +"/tools/" + "vswhere")
			)
		};
		vector<const char*> nani;
		for (auto& elem : vals)
			nani.emplace_back(elem.data());

		auto logged_string = Core::GetSystem<Application>().Exec(
			"c:/windows/system32/cmd.exe",
			nani,
			true
		);

		vector<string> tokens;

		std::stringstream stream{ logged_string };
		std::string token;
		while (std::getline(stream, token))
			tokens.emplace_back(token.data());

		tokens.erase(std::remove_if(tokens.begin(), tokens.end(), [](const string& str)
			{
				if (str.size() < 2)
					return true;

				auto sv = string_view{ str };
				sv.remove_prefix(1);
				return sv.starts_with(":\\");
			}), tokens.end());

		LOG_TO(LogPool::GAME, "Compiling game project...");
		for (auto& elem : tokens)
			LOG_TO(LogPool::GAME, elem.c_str());
	}
}