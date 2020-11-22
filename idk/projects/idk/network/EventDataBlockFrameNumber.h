#pragma once
#include <idk.h>
#include <network/network.h>
#include <network/Message.h>

namespace idk
{
	class EventDataBlockFrameNumber
		: public Message
	{
	public:
		SeqNo frame_count;
		//Host player_id;

		template <typename Stream>
		bool Serialize(Stream& stream)
		{
			//int player_id_conv = static_cast<int>(player_id);
			serialize_int(stream, frame_count.value, 0, SeqNo::max_value);
			//serialize_int(stream, player_id_conv, 0, GameConfiguration::MAX_LOBBY_MEMBERS);
			//player_id = static_cast<Host>(player_id_conv);
			return true;
		}

		NETWORK_MESSAGE_VIRTUAL_SERIALIZE_FUNCTIONS()
	};
}