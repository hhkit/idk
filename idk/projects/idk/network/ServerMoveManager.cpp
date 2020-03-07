#include "stdafx.h"
#include "ServerMoveManager.h"
#include <core/GameObject.inl>
#include <common/Transform.h>
#include <network/MoveClientMessage.h>
#include <network/ServerConnectionManager.inl>
#include <network/IDManager.h>
#include <network/NetworkSystem.h>
#include <network/ElectronView.h>
#include <network/ElectronTransformView.h>
#include <serialize/binary.inl>

namespace idk
{
	void ServerMoveManager::SubscribeEvents(ClientConnectionManager&)
	{
		IDK_ASSERT_MSG(false, "Server Move Manager only used on server!");
	}

	void ServerMoveManager::SubscribeEvents(ServerConnectionManager& server)
	{
		server.Subscribe<MoveClientMessage>([this](MoveClientMessage& msg) { OnMoveReceived(msg); });
	}

	void ServerMoveManager::OnMoveReceived(MoveClientMessage& move)
	{
		auto& id_man = Core::GetSystem<NetworkSystem>().GetIDManager();
		for (auto& move_pack : move.move_packs)
		{
			if (auto view = id_man.GetViewFromId(move_pack.network_id))
			{
				struct temp
				{
					SeqNo seq;
					vec3 val;
				};
				vector<temp> unpacked;

				for (auto& elem : move_pack.packs[0])
				{
					auto val = parse_binary<vec3>(elem.pack);
					unpacked.emplace_back(temp{ elem.seq, *val });
				}
				LOG_TO(LogPool::NETWORK, "unpacking data seq %d (%f,%f,%f)", unpacked[0].seq, unpacked[0].val.x, unpacked[0].val.y, unpacked[0].val.z);
				view->UnpackMoveData(move_pack);
			}
		}
	}
}
