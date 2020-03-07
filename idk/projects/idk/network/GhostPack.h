#pragma once
#include <idk.h>
#include <network/network.h>
namespace idk
{
	struct GhostPack
	{
		NetworkID      network_id{};
		int            state_mask{};
		vector<string> data_packs{};
	};

}