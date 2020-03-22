#pragma once
#include <yojimbo/yojimbo.h>
#include <idk.h>
#include <network/network.h>
#include <network/MovePack.h>
#include <network/yojimbo_helpers.h>
#include "GhostFlags.h"

namespace idk
{


	class MoveClientMessage
		: public yojimbo::Message
	{
	public:
		vector<MovePack> move_packs;

		template <typename Stream>
		bool Serialize(Stream& stream)
		{
			serialize_vector_count(stream, move_packs, 4096);

			for (auto& elem : move_packs)
			{
				serialize_int(stream, elem.network_id, 0, 4096);
				serialize_int(stream, elem.state_mask, 0, std::numeric_limits<int>::max());
				serialize_vector_count(stream, elem.packs, 32);
				for (auto& pack : elem.packs)
				{
					if (pack.size() > 3)
						throw;
					serialize_vector_count(stream, pack, 4);
					for (auto& seq_and_pack : pack)
					{
						serialize_int(stream, seq_and_pack.seq.value, 0, SeqNo::max_value);
						serialize_vector_count(stream, seq_and_pack.pack, 256);
						serialize_bytes(stream, (uint8_t*) seq_and_pack.pack.data(), static_cast<int>(seq_and_pack.pack.size()));
						serialize_int(stream, seq_and_pack.move_type, 0, 2);
					}
				}
			}

			return true;
		}

		YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
	};
}