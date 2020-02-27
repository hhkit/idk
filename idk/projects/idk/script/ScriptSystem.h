#pragma once
#include <idk.h>

#include <deque>
#include <mono/jit/jit.h>

#include <core/ConfigurableSystem.h>
#include <script/MonoWrapperEnvironment.h>
#include <script/MonoBehaviorEnvironment.h>

namespace idk::mono
{
	class Behavior;
	class MonoEnvironment;
	class MonoBehaviorEnvironment;

	struct ScriptSystemConfig
	{
		string path_to_game_dll = "/scripts/bin/TestAndSeek.dll";
	};

	class ScriptSystem
		: public ConfigurableSystem<ScriptSystemConfig>
	{
	public:
		bool run_scripts = true;

		ScriptSystem();
		ScriptSystem(const ScriptSystem&);
		~ScriptSystem();

		// for the sake of clarity, these update phases are done in sequence of declaration
		void ScriptStart(span<Behavior>);
		void ScriptFixedUpdate(span<Behavior>);
		void ScriptUpdate(span<Behavior>);
		void ScriptPausedUpdate(span<Behavior>);
		void ScriptUpdateCoroutines(span<Behavior>);
		void ScriptLateUpdate(span<Behavior>);

		MonoWrapperEnvironment& Environment() const;
		MonoBehaviorEnvironment& ScriptEnvironment() const;
		void HandleException(MonoObject* exc) const;

		void ApplyConfig(Config& config) override;
		void RefreshGameScripts();
		void CompileGameScripts();

		Handle<Behavior>       GetMonoBehaviorInstance(std::string_view type);
		span<Handle<Behavior>> GetMonoBehaviorsOfType(std::string_view type);
	private:
		friend class Behavior;
		string path_to_used_dll;

		hash_table<string, std::deque<Handle<Behavior>>> behavior_handles;
		unique_ptr<MonoWrapperEnvironment> main_environment;
		unique_ptr<MonoBehaviorEnvironment> script_environment;

		void LoadGameScripts();
		void UnloadGameScripts();

		void Init() override;
		void LateInit() override;
		void Shutdown() override;
	};
}