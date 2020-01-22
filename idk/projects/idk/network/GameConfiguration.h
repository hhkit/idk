#pragma once
#include <yojimbo/yojimbo.h>
namespace idk
{
	static const uint8_t DEFAULT_PRIVATE_KEY[yojimbo::KeyBytes] = { 0 };

	enum class GameMessageType {
		TEST,
		// ghost manager
		//GHOST_TFM,
		//GHOST_ACKNOWLEDGE,

		// move manager
		//MOVE_INFO,

		// event manager
		EVENT_INSTANTIATE_PREFAB,
		//EVENT_LOAD_LEVEL,
		//EVENT_CSHARP_RPC,
		//EVENT_FIRST = EVENT_INSTANTIATE_PREFAB,
		//EVENT_LAST = EVENT_CSHARP_RPC,

		// data block manager
		COUNT
	};

	enum class GameChannel {
		RELIABLE,
		UNRELIABLE,
		COUNT
	};

	struct GameConfiguration
		: yojimbo::ClientServerConfig
	{
		static constexpr auto MAX_CLIENTS = 8;
		GameConfiguration();
	};
}