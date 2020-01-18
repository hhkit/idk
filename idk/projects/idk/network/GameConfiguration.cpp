#include "stdafx.h"
#include "GameConfiguration.h"

namespace idk
{
	GameConfiguration::GameConfiguration()
		: ClientServerConfig{}
	{
		numChannels = 2;
		channel[(int)GameChannel::RELIABLE].type = yojimbo::CHANNEL_TYPE_RELIABLE_ORDERED;
		channel[(int)GameChannel::UNRELIABLE].type = yojimbo::CHANNEL_TYPE_UNRELIABLE_UNORDERED;
	}
}
