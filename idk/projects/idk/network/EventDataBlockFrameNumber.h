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

		template <typename Stream>
		bool Serialize(Stream& stream)
		{
			int val = frame_count;
			serialize_int(stream, val, 0, 0xFFFF);
			frame_count = val;
			return true;
		}

		YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
	};
}