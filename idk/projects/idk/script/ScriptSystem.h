#pragma once
#include <idk.h>

#include <deque>
#include <mono/jit/jit.h>

#include <core/ISystem.h>
#include <script/MonoBehaviorData.h>

namespace idk::mono
{
	class Behavior;
	class MonoEnvironment;
	class MonoBehaviorEnvironment;

	class ScriptSystem
		: public ISystem
	{
	public:
		string path_to_game_dll = "/scripts/bin/Debug/TestAndSeek.dll";

		ScriptSystem();
		~ScriptSystem();

		// for the sake of clarity, these update phases are done in sequence of declaration
		void ScriptStart(span<Behavior>);
		void ScriptFixedUpdate(span<Behavior>);
		void ScriptUpdate(span<Behavior>);
		void ScriptUpdateCoroutines(span<Behavior>);
		void ScriptLateUpdate(span<Behavior>);

		MonoEnvironment& Environment() const;
		MonoBehaviorEnvironment& ScriptEnvironment() const;

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
		void Shutdown() override;

		ScriptSystem(const ScriptSystem&) = delete;
		ScriptSystem(ScriptSystem&&) noexcept = delete;
		ScriptSystem& operator=(const ScriptSystem&) = delete;
		ScriptSystem& operator=(ScriptSystem&&) noexcept = delete;
	};
}