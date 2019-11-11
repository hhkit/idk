#pragma once
#include <script/MonoEnvironment.h>

namespace idk::mono
{
	class MonoBehaviorEnvironment
		: public MonoEnvironment
	{
	public:
		MonoBehaviorEnvironment(string_view full_path_to_game_dll);
		~MonoBehaviorEnvironment();
		void Init() override;
		void Execute();
		ManagedType* GetBehaviorMetadata(string_view name);
		span<const char* const> GetBehaviorList();
	private:
		vector<const char*> name_list;
		hash_table<string, ManagedType*> mono_behaviors;
		void FindMonoBehaviors();
	};
}