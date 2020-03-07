#pragma once
#include <yojimbo/yojimbo.h>
#include <idk.h>
#include <network/network.h>
#include <network/MovePack.h>
#include "GhostFlags.h"

#define serialize_vector_count(stream, vec, max_size) \
{                                                     \
	auto count = vec.size();                          \
	serialize_int(stream, count, 0, max_size);        \
	vec.resize(count);                                \
}

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
					serialize_vector_count(stream, pack, 3);
					for (auto& seq_and_pack : pack)
					{
						serialize_int(stream, seq_and_pack.seq, 0, std::numeric_limits<unsigned short>::max());
						serialize_vector_count(stream, seq_and_pack.pack, 256);
						serialize_bytes(stream, (uint8_t*) seq_and_pack.pack.data(), static_cast<int>(seq_and_pack.pack.size()));
					}
				}
			}

			return true;
		}

		YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
	};
}