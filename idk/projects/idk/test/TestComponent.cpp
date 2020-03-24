#include "stdafx.h"
#include "TestComponent.h"
#include <app/Application.h>
#include <network/NetworkSystem.h>
#include <network/ConnectionManager.inl>
#include <network/EventManager.h>
#include <network/ElectronView.inl>
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
			ElectronView::BaseParameter* param = nullptr;

			ElectronView client_ev, server_ev;
			{
				client_ev.ghost_state = std::monostate{};
				client_ev.move_state = ElectronView::MoveObject{};

				ParameterImpl<int> test_param;
				test_param.setter = [&](int val) { client_value = val; };
				test_param.getter = [&]() -> int { return client_value; };
				param = client_ev.RegisterMember("test", test_param);
			}
			{
				server_ev.ghost_state = ElectronView::Master{};
				server_ev.move_state = ElectronView::ControlObject{};

				ParameterImpl<int> test_param;
				test_param.setter = [&](int val) { server_value = val; };
				test_param.getter = [&]() -> int { return server_value; };
				server_ev.RegisterMember("test", test_param);
			}

			param->GetClientObject()->Init(SeqNo{ 0 });

			param->GetClientObject()->PushMove(SeqNo{ 0 }, SeqAndPack::set_move, 1);
			param->GetClientObject()->PushMove(SeqNo{ 1 }, SeqAndPack::set_move, 2);
			param->GetClientObject()->PushMove(SeqNo{ 2 }, SeqAndPack::set_move, 3);
			param->GetClientObject()->PushMove(SeqNo{ 2 }, SeqAndPack::set_move, 4);
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
