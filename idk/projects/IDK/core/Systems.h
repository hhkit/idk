#pragma once
#include <tuple>

namespace idk
{
	using Systems = std::tuple<
		class Application
	,	class InputSystem
	,	class ScriptSystem
	,	class PhysicsSystem
	,	class GraphicsSystem
	,	class AudioSystem
	,	class EditorSystem
	>;

	constexpr auto SystemCount = std::tuple_size_v<Systems>;
}