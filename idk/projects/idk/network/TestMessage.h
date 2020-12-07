#pragma once
#include <network/Message.h>

namespace idk
{
	class TestMessage
		: public Message 
	{
	public:
		int i = 0;

		template <typename Stream>
		bool Serialize(Stream& stream) {
			serialize_int(stream, i, 0, 512);
			return true;
		}

		NETWORK_MESSAGE_VIRTUAL_SERIALIZE_FUNCTIONS()
	};
}