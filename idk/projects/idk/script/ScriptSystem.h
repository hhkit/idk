#pragma once
#include <idk.h>

#include <deque>
#include <mono/jit/jit.h>

#include <core/ConfigurableSystem.h>
#include <script/MonoBehaviorData.h>

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
		ScriptSystem();
		ScriptSystem(const ScriptSystem&);
		~ScriptSystem();

		// for the sake of clarity, these update phases are done in sequence of declaration
		void ScriptStart(span<Behavior>);
		void ScriptFixedUpdate(span<Behavior>);
		void ScriptUpdate(span<Behavior>);
		void ScriptUpdateCoroutines(span<Behavior>);
		void ScriptLateUpdate(span<Behavior>);

		MonoEnvironment& Environment() const;
		MonoBehaviorEnvironment& ScriptEnvironment() const;

		void ApplyConfig(Config& config) override;
		void RefreshGameScripts();

		Handle<Behavior>       GetMonoBehaviorInstance(std::string_view type);
		span<Handle<Behavior>> GetMonoBehaviorsOfType(std::string_view type);
	private:
		friend class Behavior;

		hash_table<string, std::deque<Handle<Behavior>>> behavior_handles;
		unique_ptr<MonoEnvironment> main_environment;
		unique_ptr<MonoBehaviorEnvironment> script_environment;

		void LoadGameScripts();
		void UnloadGameScripts();

		void Init() override;
		void LateInit() override;
		void Shutdown() override;
	};
}