#pragma once
#include <idk.h>
#include <mono/jit/jit.h>

namespace idk::mono
{
	class MonoEnvironment
	{
	public:
		MonoEnvironment(string_view path_to_game_dll);
	private:
		MonoDomain*   script_domain   {};
		MonoAssembly* script_assembly {};
	};
}