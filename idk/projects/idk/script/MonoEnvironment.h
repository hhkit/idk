#pragma once
#include <idk.h>

#include <mono/jit/jit.h>

#include <script/MonoBehaviorData.h>

namespace idk::mono
{
	class MonoEnvironment
	{
	public:
		MonoEnvironment(string_view full_path_to_game_dll);

		~MonoEnvironment();

	private:
		MonoDomain*   script_domain   {};
		MonoAssembly* script_assembly {};

		hash_table<string, MonoBehaviorData> mono_behaviors;

		void FindMonoBehaviors();

		MonoEnvironment(MonoEnvironment&&) noexcept = delete;
		MonoEnvironment(const MonoEnvironment&) = delete;
		MonoEnvironment& operator=(MonoEnvironment&&) noexcept = delete;
		MonoEnvironment& operator=(const MonoEnvironment&) = delete;
	};
}