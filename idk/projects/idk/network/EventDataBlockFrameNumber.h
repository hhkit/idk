#pragma once
#include <yojimbo/yojimbo.h>
#include <idk.h>
#include <network/network.h>

namespace idk
{
	class EventDataBlockFrameNumber
		: public yojimbo::Message
	{
	public:
		SeqNo frame_count;
		Host player_id;

		template <typename Stream>
		bool Serialize(Stream& stream)
		{
			int player_id_conv = static_cast<int>(player_id);
			serialize_int(stream, frame_count, 0, 0xFFFF);
			serialize_int(stream, player_id_conv, 0, (int)Host::CLIENT_MAX);
			player_id = static_cast<Host>(player_id_conv);
			return true;
		}

		YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
	};
}