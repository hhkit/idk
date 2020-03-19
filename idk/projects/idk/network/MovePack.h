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

	struct MoveAck
	{
		SeqNo sequence_number { };
		unsigned ackfield     { };

		unsigned state_mask { };
		vector<string> accumulated_guesses;
	};
}