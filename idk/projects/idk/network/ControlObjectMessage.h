#pragma once
#include <idk.h>
#include <network/network.h>
#include <network/MovePack.h>
#include <network/Message.h>
#include "GhostFlags.h"

namespace idk
{
	class ControlObjectMessage
		: public Message
	{
	public:
		NetworkID network_id;
		seq_num_t move_ack;
		vector<string> control_object_data;

		template <typename Stream>
		bool Serialize(Stream& stream)
		{
			serialize_int(stream, network_id, 0, 4096);
			serialize_int(stream, move_ack.value, 0, SeqNo::max_value);
			serialize_vector_count(stream, control_object_data, 32);

			for (auto& elem : control_object_data)
			{
				serialize_vector_count(stream, elem, 4096);
				serialize_byte_stream(stream, elem);
			}

			return true;
		}

		NETWORK_MESSAGE_VIRTUAL_SERIALIZE_FUNCTIONS()
	};
}