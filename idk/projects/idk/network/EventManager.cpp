#include "stdafx.h"
#include "EventManager.h"

#include <prefab/Prefab.h>
#include <scene/SceneManager.h>
#include <core/GameObject.inl>
#include <common/Transform.h>

#include <network/ConnectionManager.inl>
#include <network/TestMessage.h>
#include <network/NetworkSystem.h>
#include <network/IDManager.h>
#include <network/ElectronView.h>
#include <network/ElectronTransformView.h>
#include <network/EventInstantiatePrefabMessage.h>

namespace idk
{
	void EventManager::SubscribeEvents(ClientConnectionManager& client)
	{
		client.Subscribe<TestMessage>([](TestMessage* message) { LOG_TO(LogPool::NETWORK, "Received message %d", message->i); });
		client.Subscribe<EventInstantiatePrefabMessage>([this](EventInstantiatePrefabMessage* msg) { OnInstantiatePrefabEvent(msg); });
	}

	void EventManager::SubscribeEvents(ServerConnectionManager& server)
	{
		server.Subscribe<TestMessage>([&server](TestMessage* message)
			{
				LOG_TO(LogPool::NETWORK, "Received message %d", message->i);
				auto pingback = server.CreateMessage<TestMessage>();
				pingback->i = message->i + 1;
				server.SendMessage(pingback, GameChannel::RELIABLE);
			});

		// the server should never be told to instantiate prefabs
		//server.Subscribe<EventInstantiatePrefabMessage>([this](EventInstantiatePrefabMessage* msg) { OnInstantiatePrefabEvent(msg); });
	}
	void EventManager::SendTestMessage(int i)
	{
		auto& conn_man = Core::GetSystem<NetworkSystem>().GetConnectionManager();
		auto test_mess = conn_man.CreateMessage<TestMessage>();
		test_mess->i = i;
		conn_man.SendMessage(test_mess, GameChannel::RELIABLE);
	}

	void EventManager::SendInstantiatePrefabEvent(RscHandle<Prefab> prefab, opt<vec3> position, opt<quat> rotation)
	{
		auto dyn = prefab->data[0].FindComponent("ElectronView", 0);
		if (!dyn.valid())
		{
			LOG_TO(LogPool::NETWORK, "Tried to instantiate a prefab without an ElectronView component!");
			return;
		}

		auto obj = prefab->Instantiate(*Core::GetSystem<SceneManager>().GetActiveScene());
		auto ev = obj->GetComponent<ElectronView>();
		Core::GetSystem<NetworkSystem>().GetIDManager().CreateNewIDFor(ev);
		auto& tfm = *obj->Transform();
		if (position)
			tfm.position = *position;
		if (rotation)
			tfm.rotation = *rotation;

		auto instantiate_event = connection_manager->CreateMessage<EventInstantiatePrefabMessage>();
		instantiate_event->id = ev->network_id;
		instantiate_event->prefab = prefab;

		if (position)
		{
			instantiate_event->use_position = true;
			instantiate_event->position = *position;
		}
		if (rotation)
		{
			instantiate_event->use_rotation = true;
			instantiate_event->rotation = *rotation;
		}

		if (auto tfm_view = obj->GetComponent<ElectronTransformView>())
		{
			tfm_view->ghost_data = ElectronTransformView::PreviousFrame
			{
				.position = tfm.position,
				.rotation = tfm.rotation,
				.scale = tfm.scale,
				.state_mask = 0
			};
		}

		connection_manager->SendMessage(instantiate_event, GameChannel::RELIABLE);

#pragma message("Store the event for future connections")
	}

	void EventManager::OnInstantiatePrefabEvent(EventInstantiatePrefabMessage* message)
	{
		auto obj = message->prefab->Instantiate(*Core::GetSystem<SceneManager>().GetActiveScene());
		auto ev = obj->GetComponent<ElectronView>();
		IDK_ASSERT(Core::GetSystem<NetworkSystem>().GetIDManager().EmplaceID(message->id, ev));
		auto& tfm = *obj->Transform();
		if (message->use_position)
			tfm.position = message->position;
		if (message->use_rotation)
			tfm.rotation = message->rotation;
	}
}
