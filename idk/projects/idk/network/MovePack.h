#pragma once

#include <idk.h>
#include <network/network.h>

namespace idk
{
	struct SeqAndPack 
	{ 
		SeqNo seq; 
		string pack; 
	};

	struct MovePack
	{
		using MoveList = small_vector<SeqAndPack>;

		NetworkID network_id{};
		unsigned state_mask{};
		vector<MoveList> packs;
	};

	struct ControlGhost
	{
		NetworkID network_id;
		SeqNo sequence_number { };
		unsigned ackfield     { };

		unsigned state_mask { };
		vector<string> verified_ghost_value;
	};
}