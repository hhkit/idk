#pragma once
#include <yojimbo/yojimbo.h>
namespace idk
{
	static const uint8_t DEFAULT_PRIVATE_KEY[yojimbo::KeyBytes] = { 0 };

	enum class GameMessageType {
		TEST,
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
		GameConfiguration();
	};
}