#pragma once
#include <limits>
#include <idk.h>
#include <network/network.h>
#include <network/yojimbo_helpers.h>
#include <network/MovePack.h>
#include <network/GhostPack.h>

namespace idk
{
	struct GhostWithMoveAck
		: GhostPack
	{
		MoveAck ack;
	};

	class MoveAcknowledgementMessage
		: public yojimbo::Message
	{
	public:

		vector<GhostWithMoveAck> objects;

		template <typename Stream>
		bool Serialize(Stream& stream)
		{
			serialize_vector_count(stream, objects, 4096);
			for (auto& elem : objects)
			{
				serialize_int(stream, elem.network_id, 0, 4096);
				serialize_int(stream, elem.state_mask, 0, std::numeric_limits<unsigned>::max());
				serialize_int(stream, elem.ack.sequence_number.value, 0, SeqNo::max_value);
				serialize_int(stream, elem.ack.ackfield, 0, std::numeric_limits<unsigned>::max());

				serialize_int(stream, elem.ack.state_mask, 0, std::numeric_limits<unsigned>::max());
				serialize_vector_count(stream, elem.ack.accumulated_guesses, 32);
				for (auto& guess : elem.ack.accumulated_guesses)
				{
					serialize_vector_count(stream, guess, 2048);
					serialize_bytes(stream, (uint8_t*) guess.data(), guess.size());
				}

				serialize_vector_count(stream, elem.data_packs, 64);
				for (auto& pack : elem.data_packs)
				{
					serialize_vector_count(stream, pack, 0xFFFF);
					serialize_bytes(stream, (uint8_t*)pack.data(), pack.size());
				}
			}

			return true;
		}

		YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
	};
}