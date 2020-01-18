#pragma once
#include <yojimbo/yojimbo.h>
namespace idk
{
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