#pragma once
#include <mono/jit/jit.h>

namespace idk
{
	struct MonoBehaviorData
	{
		MonoClass* klass = nullptr;
		bool is_trigger_enter = false;
		bool is_trigger_stay = false;
		bool is_trigger_exit = false;
		//bool is_collision_enter = false;
	};
}