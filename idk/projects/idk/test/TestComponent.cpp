#include "stdafx.h"
#include "TestComponent.h"
#include <app/Application.h>
#include <network/NetworkSystem.h>
#include <network/ConnectionManager.inl>
#include <network/EventManager.h>
#include <network/ElectronView.inl>
#include <network/MoveAcknowledgementMessage.h>
#include <network/ack_utils.h>
#include <core/GameObject.h>
#include <common/Transform.h>
namespace idk
{
	void TestComponent::NetworkUpdate()
	{
		auto& app_sys = Core::GetSystem<Application>();
		auto& network_sys = Core::GetSystem<NetworkSystem>();

		if (app_sys.GetKeyDown(Key::T))
		{
			int client_value = 0;
			int server_value = 0;


			ElectronView client_ev, server_ev;
			{
				client_ev.ghost_state = std::monostate{};
				client_ev.move_state = ElectronView::ClientObject{};

				ParameterImpl<int> test_param;
				test_param.setter = [&](int val) { client_value = val; };
				test_param.getter = [&]() -> int { return client_value; };
				client_ev.RegisterMember("test", test_param);
			}
			{
				server_ev.ghost_state = ElectronView::Master{};
				server_ev.move_state = ElectronView::ControlObject{};

				ParameterImpl<int> test_param;
				test_param.setter = [&](int val) { server_value = val; };
				test_param.getter = [&]() -> int { return server_value; };
				server_ev.RegisterMember("test", test_param);
			}

			std::optional<MovePack> client_to_server_pack;
			std::optional<GhostWithMoveAck> server_to_client_pack;

			// init

			for (auto& elem : client_ev.parameters)
			{
				elem->GetGhost()->value_index = Core::GetSystem<NetworkSystem>().GetSequenceNumber();
				elem->GetMaster()->last_packed = Core::GetSystem<NetworkSystem>().GetSequenceNumber();
			}

			for (auto& elem : server_ev.parameters)
			{
				elem->GetGhost()->value_index = Core::GetSystem<NetworkSystem>().GetSequenceNumber();
				elem->GetMaster()->last_packed = Core::GetSystem<NetworkSystem>().GetSequenceNumber();
			}

			for (unsigned frame = 0; frame < 60; ++frame)
			{
				// receive packets
				if (server_to_client_pack)
				{
					auto acks = ackfield_to_acks(server_to_client_pack->ack.sequence_number, server_to_client_pack->ack.ackfield);
					client_ev.ReceiveMoveAcknowledgements(1, acks);
					client_ev.UnpackGhostData(SeqNo{ frame }, *server_to_client_pack);
					server_to_client_pack.reset();
				}
				if (client_to_server_pack)
				{
					server_ev.UnpackMoveData(*client_to_server_pack);
					client_to_server_pack.reset();
				}

				// frame update
				++client_value;

				// send client packets
				client_ev.PrepareDataForSending();
				server_ev.PrepareDataForSending();


				client_to_server_pack = client_ev.PackMoveData();
				auto server_frame = server_ev.PrepareMoveAcknowledgements(SeqNo{ frame });
				auto server_packs = server_ev.MasterPackData(1);
				
				server_to_client_pack = GhostWithMoveAck
				{
					server_packs,
					server_frame
				};
			}
		}

		if (app_sys.GetKeyDown(Key::P))
		{
			EventManager::BroadcastInstantiatePrefab(makeme,
				send_pos ? opt<vec3>{GetGameObject()->Transform()->GlobalPosition()} : std::nullopt,
				send_rot ? opt<quat>{GetGameObject()->Transform()->GlobalRotation()} : std::nullopt);
		}

		if (app_sys.GetKeyDown(Key::L))
		{
			auto view = GetGameObject()->GetComponent<ElectronView>();
			if (view)
			{
				if (auto conn_man = network_sys.GetConnectionTo(Host::CLIENT0))
					conn_man->GetManager<EventManager>()->SendTransferOwnership(view, Host::CLIENT0);
			}
		}
	}
}
