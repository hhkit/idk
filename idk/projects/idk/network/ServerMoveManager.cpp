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
namespace idk
{
	void ServerMoveManager::SubscribeEvents(ClientConnectionManager& client)
	{
		IDK_ASSERT(false, "Server Move Manager only used on server!");
	}

	void ServerMoveManager::SubscribeEvents(ServerConnectionManager& server)
	{
		server.Subscribe<MoveClientMessage>([this](MoveClientMessage* msg) { OnMoveReceived(msg); });
	}

	void ServerMoveManager::OnMoveReceived(MoveClientMessage* move)
	{
		auto view = Core::GetSystem<NetworkSystem>().GetIDManager().GetViewFromId(move->network_id);
		if (view)
		{
			auto go = view->GetGameObject();
			auto tfm_view = go->GetComponent<ElectronTransformView>();
			if (tfm_view->sync_position)
			{
				if (auto trans = move->GetTranslation())
					go->Transform()->position += *trans;
			}

			if (tfm_view->sync_rotation)
			{
				if (auto rot = move->GetRotation())
				{
					auto& r = go->Transform()->rotation;
					r = *rot * r;
				}
			}


			if (tfm_view->sync_scale)
			{
				if (auto scale = move->GetScale())
					go->Transform()->scale *= *scale;
			}
		}
	}
}
