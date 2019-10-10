#pragma once
#include <idk.h>

#include <deque>
#include <mono/jit/jit.h>

#include <core/ISystem.h>
#include <script/MonoBehaviorData.h>

namespace idk
{
	class MonoBehavior;

	class ScriptSystem
		: public ISystem
	{
	public:
		string path_to_game_dll = "";

		// for the sake of clarity, these update phases are done in sequence of declaration
		void ScriptStart(span<MonoBehavior>);
		void ScriptFixedUpdate(span<MonoBehavior>);
		void ScriptUpdate(span<MonoBehavior>);
		void ScriptUpdateCoroutines(span<MonoBehavior>);
		void ScriptLateUpdate(span<MonoBehavior>);

		MonoBehaviorData* GetMonoBehaviorType(std::string_view);
		const hash_table<string, MonoBehaviorData>& GetMonoBehaviorDataList();

		MonoDomain* GetScriptDomain();
		MonoImage* GetLibImage();
		MonoClass* GetClassOfType(std::string_view type_name);

		void RefreshGameScripts();

		Handle<MonoBehavior>       GetMonoBehaviorInstance(std::string_view type);
		span<Handle<MonoBehavior>> GetMonoBehaviorsOfType(std::string_view type);
	private:
		friend class MonoBehavior;
		MonoDomain* domain = nullptr;
		MonoAssembly* lib_assembly = nullptr;

		hash_table<string, MonoBehaviorData>          mono_behaviors;
		hash_table<string, std::deque<Handle<MonoBehavior>>> behavior_handles;

		void FindMonoBehaviors();
		void ClearMonoBehaviors();

		void LoadGameScripts();
		void UnloadGameScripts();
		bool ImplementsInterface(MonoClass*, std::string_view);

		void Init() override;
		void Shutdown() override;
	};
}