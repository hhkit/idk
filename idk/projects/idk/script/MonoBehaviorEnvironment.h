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
		opt<ManagedType> GetBehaviorMetadata(string_view name);
	private:
		hash_table<string, ManagedType> mono_behaviors;
		void Execute();
		void FindMonoBehaviors();
	};
}