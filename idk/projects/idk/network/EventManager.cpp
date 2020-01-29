#include "stdafx.h"
#include "EventManager.h"

#include <prefab/Prefab.h>
#include <scene/SceneManager.h>
#include <core/GameObject.inl>
#include <common/Transform.h>

#include <network/ConnectionManager.inl>
#include <network/TestMessage.h>
#include <network/NetworkSystem.inl>
#include <network/IDManager.h>
#include <network/ElectronView.h>
#include <network/ElectronTransformView.h>
#include <network/EventInstantiatePrefabMessage.h>
#include <network/EventTransferOwnershipMessage.h>

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
				server.CreateAndSendMessage<TestMessage>(GameChannel::RELIABLE, [&](TestMessage& msg)
				{
					msg.i = message->i + 1;
				});
			});

		// the server should never be told to instantiate prefabs
		//server.Subscribe<EventInstantiatePrefabMessage>([this](EventInstantiatePrefabMessage* msg) { OnInstantiatePrefabEvent(msg); });
	}
	void EventManager::SendTestMessage(int i)
	{
		Core::GetSystem<NetworkSystem>().BroadcastMessage <TestMessage>(GameChannel::RELIABLE, [&](TestMessage& msg)
			{
				msg.i = i;
			});
	}

	void EventManager::BroadcastInstantiatePrefab(RscHandle<Prefab> prefab, opt<vec3> position, opt<quat> rotation)
	{
		auto dyn = prefab->data[0].FindComponent("ElectronView", 0);
		if (!dyn.valid())
		{
			LOG_TO(LogPool::NETWORK, "Tried to instantiate a prefab without an ElectronView component!");
			return;
		}

		// create object
		auto obj = prefab->Instantiate(*Core::GetSystem<SceneManager>().GetActiveScene());
		auto ev = obj->GetComponent<ElectronView>();
		Core::GetSystem<NetworkSystem>().GetIDManager().CreateNewIDFor(ev);
		auto& tfm = *obj->Transform();
		if (position)
			tfm.position = *position;
		if (rotation)
			tfm.rotation = *rotation;
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


		Core::GetSystem<NetworkSystem>().BroadcastMessage<EventInstantiatePrefabMessage>(GameChannel::RELIABLE,
			[&](EventInstantiatePrefabMessage& msg)
			{
				msg.id = ev->network_id;
				msg.prefab = prefab;

				if (position)
				{
					msg.use_position = true;
					msg.position = *position;
				}
				if (rotation)
				{
					msg.use_rotation = true;
					msg.rotation = *rotation;
				}
			});

#pragma message("Store the event for future connections")
	}

	// static
	void EventManager::SendTransferOwnership(Handle<ElectronView> transfer, Host target_host)
	{
		auto conn_man = Core::GetSystem<NetworkSystem>().GetConnectionTo(target_host);
		if (!conn_man)
		{
			LOG_TO(LogPool::NETWORK, "Tried to transfer to a nonexistent host");
			return;
		}
		
		if (!transfer)
		{
			LOG_TO(LogPool::NETWORK, "Tried to transfer an invalid object");
			return;
		}

		transfer->owner = target_host;

		conn_man->CreateAndSendMessage<EventTransferOwnershipMessage>(GameChannel::RELIABLE, 
			[&](EventTransferOwnershipMessage& msg)
			{
				msg.object_to_transfer = transfer->network_id;
			});
	}

	// targets
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
