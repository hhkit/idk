#pragma once
#include <yojimbo/yojimbo.h>
#include <idk.h>
#include <network/network.h>
#include <network/GhostFlags.h>
#include <network/GhostPack.h>
namespace idk
{
	class GhostMessage
		: public yojimbo::Message
	{
	public:
		SeqNo sequence_number;
		vector<GhostPack> ghost_packs;

		template <typename Stream>
		bool Serialize(Stream& stream)
		{
			serialize_int(stream, sequence_number.value, 0, SeqNo::max_value);
			auto count = ghost_packs.size();
			serialize_int(stream, count, 0, 4096);
			ghost_packs.resize(count);

			for (auto& elem : ghost_packs)
			{
				serialize_int(stream, elem.network_id, 0, 4096);
				serialize_int(stream, elem.state_mask, 0, std::numeric_limits<int>::max());

				auto sm = elem.state_mask;
				auto packs = 0;
				while (sm)
				{
					++packs;
					sm >>= 1;
				}

				elem.data_packs.resize(packs);
				for (auto& ghost_pack : elem.data_packs)
				{
					auto string_sz = static_cast<int>(ghost_pack.size());
					serialize_int(stream, string_sz, 0, 511);
					ghost_pack.resize(string_sz);
					serialize_bytes(stream, (uint8_t*) ghost_pack.data(), string_sz);
				}
			}

			return true;
		}

		YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
	};
}