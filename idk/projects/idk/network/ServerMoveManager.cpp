#include "stdafx.h"
#include "ServerMoveManager.h"
#include <core/GameObject.inl>
#include <common/Transform.h>
#include <network/MoveClientMessage.h>
#include <network/ControlObjectMessage.h>
#include <network/ServerConnectionManager.inl>
#include <network/IDManager.h>
#include <network/NetworkSystem.h>
#include <network/ElectronView.h>
#include <network/ElectronTransformView.h>
#include <serialize/binary.inl>
#include "ConnectionManager.inl"
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

	void ServerMoveManager::SendControlObjects(span<ElectronView> views)
	{
		const auto target = connection_manager->GetConnectedHost();

		for (auto& ev : views)
		{
			if (ev.owner == target)
			{
				// create 
				auto& inputs = std::get<ElectronView::ServerSideInputs>(ev.move_state);
				const auto move_ack = inputs.moves.base();

				vector<string> packs;
				for (auto& elem : ev.GetParameters())
					packs.emplace_back(elem->GetMaster()->PackData());

				connection_manager->CreateAndSendMessage<ControlObjectMessage>(GameChannel::FASTEST_GUARANTEED, [&](ControlObjectMessage& msg)
				{
					msg.network_id = ev.network_id;
					msg.move_ack = move_ack;
					msg.control_object_data = std::move(packs);
				});
			}
		}
	}

	void ServerMoveManager::OnMoveReceived(MoveClientMessage& move)
	{
		const auto& id_man = Core::GetSystem<NetworkSystem>().GetIDManager();
		for (auto& move_pack : move.move_packs)
		{
			if (auto view = id_man.GetViewFromId(move_pack.network_id))
			{
			//	LOG_TO(LogPool::NETWORK, "unpacking move for %d", move_pack.network_id);
				view->UnpackMoveData(move_pack);
			}
		}
	}
}
