#pragma once
#include <tuple>
#include <core/Scheduler.h>

namespace idk
{
	constexpr auto UnpauseAll  = PausedSystemConfig<>{};
	constexpr auto EditorPause = PausedSystemConfig<PhysicsSystem, mono::ScriptSystem, AnimationSystem>{};
	constexpr auto GamePause   = PausedSystemConfig<PhysicsSystem, AnimationSystem>{};
}