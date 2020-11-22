#pragma once
#include <idk.h>
#include <network/network.h>
#include <network/Message.h>

namespace idk
{
	class EventTransferOwnershipMessage
		: public Message
	{
	public:
		NetworkID object_to_transfer = 0;

		template <typename Stream>
		bool Serialize(Stream& stream)
		{
			serialize_int(stream, object_to_transfer, 0, 4096);
			return true;
		}

		NETWORK_MESSAGE_VIRTUAL_SERIALIZE_FUNCTIONS()
	};
}