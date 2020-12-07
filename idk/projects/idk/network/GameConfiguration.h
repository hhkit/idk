#pragma once

namespace idk
{
	enum class GameChannel {
		FASTEST_GUARANTEED,
		RELIABLE,
		UNRELIABLE,
		COUNT
	};

	struct GameConfiguration
	{
		static constexpr auto MAX_CLIENTS = 3;
		static constexpr auto MAX_LOBBY_MEMBERS = 4;
	};
}