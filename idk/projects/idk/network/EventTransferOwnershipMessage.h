#pragma once
#include <yojimbo/yojimbo.h>
#include <idk.h>
#include <network/network.h>

namespace idk
{
	class EventTransferOwnershipMessage
		: public yojimbo::Message
	{
	public:
		NetworkID object_to_transfer = 0;

		template <typename Stream>
		bool Serialize(Stream& stream)
		{
			serialize_int(stream, object_to_transfer, 0, 4096);
			return true;
		}

		YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
	};
}