#pragma once
#include <yojimbo/yojimbo.h>
#include <idk.h>
#include <network/network.h>
#include <network/GhostFlags.h>
namespace idk
{
	class GhostMessage
		: public yojimbo::Message
	{
	public:
		unsigned network_id;
		SeqNo sequence_number;

		StateMask state_mask{};
		vector<string> pack;

		template <typename Stream>
		bool Serialize(Stream& stream)
		{
			serialize_int(stream, network_id, 0, 4096);
			serialize_int(stream, sequence_number, 0, 65535);
			serialize_int(stream, state_mask, 0, std::numeric_limits<int>::max());

			{
				auto tmp_state_mask = state_mask;
				unsigned count = 0;
				while (tmp_state_mask)
				{
					if (tmp_state_mask & 0x1)
						++count;
					tmp_state_mask >>= 1;
				}
				pack.resize(count);
			}
			for (auto& elem : pack)
			{
				unsigned sz = s_cast<unsigned>(elem.size());
				serialize_uint32(stream, sz);
				elem.resize(sz);
				serialize_bytes(stream, (uint8_t*)elem.data(), sz);
			}

			return true;
		}

		YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
	};
}