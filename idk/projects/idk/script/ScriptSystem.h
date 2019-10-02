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
		friend class MonoBehavior;
		MonoDomain* domain = nullptr;
		MonoDomain* script_domain = nullptr;
		MonoAssembly* script_assembly = nullptr;
		MonoAssembly* lib_assembly = nullptr;

		hash_table<string, MonoBehaviorData> mono_behaviors;
		hash_table<string, std::deque<MonoBehavior*>> behavior_handles;

		void RegisterMonoBehavior(MonoBehavior*);
		void DeregisterMonoBehavior(MonoBehavior*);

		void FindMonoBehaviors();
		void ClearMonoBehaviors();

		void LoadGameScripts();
		void UnloadGameScripts();
		bool ImplementsInterface(MonoClass*, std::string_view);
	public:
		string path_to_game_dll = "";

		void ScriptAwaken(span<MonoBehavior>);
		void ScriptUpdate(span<MonoBehavior>);
		void ScriptFixedUpdate(span<MonoBehavior>);

		MonoBehaviorData* GetMonoBehaviorType(std::string_view);
		const hash_table<string, MonoBehaviorData>& GetMonoBehaviorDataList();

		MonoDomain* GetScriptDomain();
		MonoImage* GetLibImage();
		MonoClass* GetClassOfType(std::string_view type_name);

		void RefreshGameScripts();

		MonoBehavior* GetMonoBehaviorInstance(std::string_view type);
		span<MonoBehavior*> GetMonoBehaviorsOfType(std::string_view type);
	private:
		void Init() override;
		void Shutdown() override;
	};
}