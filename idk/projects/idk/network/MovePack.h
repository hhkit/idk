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
		int state_mask{};
		vector<MoveList> packs;
	};
}