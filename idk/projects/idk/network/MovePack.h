#pragma once

#include <idk.h>
#include <network/network.h>

namespace idk
{
	struct SeqAndPack 
	{
		static constexpr auto set_move = 0;
		static constexpr auto delta_move = 1;
		static constexpr auto custom_move = 2;
		static constexpr auto move_limit = 3;

		SeqNo  seq; 
		string pack; 
		int    move_type = set_move;
		int    send_count = move_limit;
	};

	struct MovePack
	{
		using MoveList = small_vector<SeqAndPack>;

		NetworkID network_id{};
		unsigned state_mask{};
		vector<MoveList> packs;
	};
}