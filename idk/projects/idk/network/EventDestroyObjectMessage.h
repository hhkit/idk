#pragma once
#include <idk.h>
#include <network/network.h>
#include <network/Message.h>
#include <res/ResourceHandle.h>

namespace idk
{
	class EventDestroyObjectMessage
		: public Message
	{
	public:
		NetworkID id;

		template <typename Stream>
		bool Serialize(Stream& stream)
		{
			serialize_int(stream, id, 0, 4096);
			return true;
		}

		NETWORK_MESSAGE_VIRTUAL_SERIALIZE_FUNCTIONS()
	};

}