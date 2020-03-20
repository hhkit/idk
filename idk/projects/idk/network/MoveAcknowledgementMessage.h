#pragma once
#include <limits>
#include <idk.h>
#include <network/network.h>
#include <network/yojimbo_helpers.h>
#include <network/MovePack.h>
#include <network/GhostPack.h>

namespace idk
{
	class MoveAcknowledgementMessage
		: public yojimbo::Message
	{
	public:

		vector<ControlGhost> objects;

		template <typename Stream>
		bool Serialize(Stream& stream)
		{
			serialize_vector_count(stream, objects, 4096);
			for (auto& elem : objects)
			{
				serialize_int(stream, elem.network_id, 0, 4096);
				serialize_int(stream, elem.sequence_number.value, 0, SeqNo::max_value);
				serialize_int(stream, elem.ackfield, 0, std::numeric_limits<unsigned>::max());

				serialize_int(stream, elem.state_mask, 0, std::numeric_limits<unsigned>::max());

				serialize_vector_count(stream, elem.verified_ghost_value, 32);
				for (auto& ghost : elem.verified_ghost_value)
				{
					serialize_vector_count(stream, ghost, 2048);
					serialize_bytes(stream, (uint8_t*) ghost.data(), ghost.size());
				}
			}

			return true;
		}

		YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
	};
}