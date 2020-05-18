#pragma once

#include <idk.h>
#include <network/network.h>

namespace idk
{
	struct SeqAndMove
	{
		SeqNo  seq; 
		string move;
	};

	struct MovePack
	{
		using MoveList = small_vector<SeqAndMove>;

		NetworkID network_id{};
		small_vector<SeqAndMove> packs;
	};
}