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

	class ScriptSystem
		: public ISystem
	{
	public:
		string path_to_game_dll = "/scripts/hydenseek.dll";

		ScriptSystem();
		~ScriptSystem();

		// for the sake of clarity, these update phases are done in sequence of declaration
		void ScriptStart(span<Behavior>);
		void ScriptFixedUpdate(span<Behavior>);
		void ScriptUpdate(span<Behavior>);
		void ScriptUpdateCoroutines(span<Behavior>);
		void ScriptLateUpdate(span<Behavior>);

		MonoBehaviorData* GetMonoBehaviorType(std::string_view);
		const hash_table<string, MonoBehaviorData>& GetMonoBehaviorDataList();

		MonoAssembly* GetLibrary() const;
		MonoDomain* GetScriptDomain();
		MonoImage*  GetLibImage();
		MonoClass*  GetClassOfType(std::string_view type_name);

		void RefreshGameScripts();

		Handle<Behavior>       GetMonoBehaviorInstance(std::string_view type);
		span<Handle<Behavior>> GetMonoBehaviorsOfType(std::string_view type);
	private:
		friend class Behavior;
		MonoDomain*   domain = nullptr;
		MonoAssembly* lib_assembly = nullptr;

		hash_table<string, std::deque<Handle<Behavior>>> behavior_handles;
		unique_ptr<MonoEnvironment> environment;

		void FindMonoBehaviors();
		void ClearMonoBehaviors();

		void LoadGameScripts();
		void UnloadGameScripts();
		bool ImplementsInterface(MonoClass*, std::string_view);

		void Init() override;
		void Shutdown() override;

		ScriptSystem(const ScriptSystem&) = delete;
		ScriptSystem(ScriptSystem&&) noexcept = delete;
		ScriptSystem& operator=(const ScriptSystem&) = delete;
		ScriptSystem& operator=(ScriptSystem&&) noexcept = delete;
	};
}