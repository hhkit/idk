#include "stdafx.h"
#include "EventManager.h"

#include <prefab/Prefab.h>
#include <scene/SceneManager.h>
#include <res/ResourceHandle.inl>
#include <core/GameObject.inl>
#include <common/Transform.h>

#include <phys/RigidBody.h>

#include <network/ConnectionManager.inl>
#include <network/TestMessage.h>
#include <network/NetworkSystem.inl>
#include <network/IDManager.h>
#include <network/ElectronView.h>
#include <network/ElectronRigidbodyView.h>
#include <network/ElectronTransformView.h>
#include <network/EventLoadLevelMessage.h>
#include <network/EventDestroyObjectMessage.h>
#include <network/EventInstantiatePrefabMessage.h>
#include <network/EventTransferOwnershipMessage.h>
#include <network/EventInvokeRPCMessage.h>

namespace idk
{
	void EventManager::SubscribeEvents(ClientConnectionManager& client)
	{
		client.Subscribe<TestMessage>([](TestMessage& message) { LOG_TO(LogPool::NETWORK, "Received message %d", message.i); });
		client.Subscribe<EventInstantiatePrefabMessage>([this](EventInstantiatePrefabMessage& msg) { OnInstantiatePrefabEvent(msg); });
		client.Subscribe<EventTransferOwnershipMessage>([this](EventTransferOwnershipMessage& msg) { OnTransferOwnershipEvent(msg); });
		client.Subscribe<EventLoadLevelMessage>([this](EventLoadLevelMessage& msg) { OnLoadLevelMessage(msg); });
		client.Subscribe<EventInvokeRPCMessage>([this](EventInvokeRPCMessage& msg) { OnInvokeRPCMessage(msg); });
		client.Subscribe<EventDestroyObjectMessage>([this](EventDestroyObjectMessage& msg) {OnDestroyObjectMessage(msg); });
	}

	void EventManager::SubscribeEvents(ServerConnectionManager& server)
	{
		server.Subscribe<TestMessage>([&server](TestMessage& message)
			{
				LOG_TO(LogPool::NETWORK, "Received message %d", message.i);
				server.CreateAndSendMessage<TestMessage>(GameChannel::RELIABLE, [&](TestMessage& msg)
				{
					msg.i = message.i + 1;
				});
			});
		server.Subscribe<EventInvokeRPCMessage>([this](EventInvokeRPCMessage& msg) { OnInvokeRPCMessage(msg); });

		// the server should never be told to instantiate prefabs
		//server.Subscribe<EventInstantiatePrefabMessage>([this](EventInstantiatePrefabMessage* msg) { OnInstantiatePrefabEvent(msg); });
	}
	void EventManager::SendTestMessage(int i)
	{
		LOG_TO(LogPool::NETWORK, "Sending test message w payload %d", i);
		Core::GetSystem<NetworkSystem>().BroadcastMessage <TestMessage>(GameChannel::RELIABLE, [&](TestMessage& msg)
			{
				msg.i = i;
			});
	}


	Handle<GameObject> EventManager::BroadcastInstantiatePrefab(RscHandle<Prefab> prefab, opt<vec3> position, opt<quat> rotation)
	{
		LOG_TO(LogPool::NETWORK, "Broadcasting instantiate prefab message %s", string(prefab.guid).c_str());
		if (!prefab)
			return {};

		auto dyn = prefab->data[0].FindComponent("ElectronView", 0);
		if (!dyn.valid())
		{
			LOG_TO(LogPool::NETWORK, "Tried to instantiate a prefab without an ElectronView component!");
			return {};
		}

		// create object
		auto obj = prefab->Instantiate(*Core::GetSystem<SceneManager>().GetActiveScene());
		auto ev = obj->GetComponent<ElectronView>();
		Core::GetSystem<NetworkSystem>().GetIDManager().CreateNewIDFor(ev);
		ev->Setup();
		auto& tfm = *obj->Transform();
		if (position)
			tfm.position = *position;
		if (rotation)
			tfm.rotation = *rotation;

		ev->ghost_state = ElectronView::Master{};

		Core::GetSystem<NetworkSystem>().BroadcastMessage<EventInstantiatePrefabMessage>(GameChannel::RELIABLE,
			[&](EventInstantiatePrefabMessage& msg)
			{
				msg.id = ev->network_id;
				msg.prefab = prefab;

				if (position)
				{
					msg.use_position = true;
					msg.position = tfm.position;
				}
				if (rotation)
				{
					msg.use_rotation = true;
					for (unsigned i = 0; i < 4; ++i)
						msg.rotation[i] = tfm.rotation[i];
				}
			});

		return obj;
#pragma message("Store the event for future connections")
	}

	// static
	void EventManager::SendTransferOwnership(Handle<ElectronView> transfer, Host target_host)
	{
		LOG_TO(LogPool::NETWORK, "Transferring ownership of %d to %d", transfer->network_id, (int) target_host);
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
		transfer->move_state = ElectronView::ControlObject{};

		const auto go = transfer->GetGameObject();

		conn_man->CreateAndSendMessage<EventTransferOwnershipMessage>(GameChannel::RELIABLE, 
			[&](EventTransferOwnershipMessage& msg)
			{
				msg.object_to_transfer = transfer->network_id;
			});
	}

	void EventManager::BroadcastLoadLevel(RscHandle<Scene> scene)
	{
		Core::GetSystem<SceneManager>().SetNextScene(scene);

		Core::GetSystem<SceneManager>().OnSceneChange.Listen(
			[&id_manager = Core::GetSystem<NetworkSystem>().GetIDManager()]
		(RscHandle<Scene> scene)
		{
			for (auto& elem : Core::GetGameState().GetObjectsOfType<ElectronView>())
			{
				id_manager.CreateNewIDFor(elem.GetHandle());
				elem.Setup();
				elem.ghost_state = ElectronView::Master{};
				elem.state_mask = 0;
			}

			Core::GetSystem<NetworkSystem>().BroadcastMessage<EventLoadLevelMessage>(GameChannel::RELIABLE,
				[scene](EventLoadLevelMessage& msg)
				{
					msg.SetScene(scene);
					for (auto& elem : Core::GetGameState().GetObjectsOfType<ElectronView>())
						msg.AddView(elem.GetHandle());
				});
		}
		, 1); // fire once
	}

	void EventManager::BroadcastDestroyView(Handle<ElectronView> view)
	{
		LOG_TO(LogPool::NETWORK, "Destroy item");
		auto id = view->network_id;
		Core::GetSystem<NetworkSystem>().BroadcastMessage<EventDestroyObjectMessage>(GameChannel::RELIABLE, [&](EventDestroyObjectMessage& msg)
			{
				msg.id = id;
			});

		Core::GetGameState().DestroyObject(view->GetGameObject());
	}

	// targets
	void EventManager::OnInstantiatePrefabEvent(EventInstantiatePrefabMessage& message)
	{
		LOG_TO(LogPool::NETWORK, "Received instantiate prefab event");
		if (!message.prefab)
		{
			LOG_TO(LogPool::NETWORK, "Prefab %s does not exist", static_cast<string>(message.prefab.guid).c_str());
			return;
		}

		auto obj = message.prefab->Instantiate(*Core::GetSystem<SceneManager>().GetActiveScene());
		auto ev = obj->GetComponent<ElectronView>();

		// set to ghost state
		ev->ghost_state = ElectronView::Ghost{};

		IDK_ASSERT(Core::GetSystem<NetworkSystem>().GetIDManager().EmplaceID(message.id, ev));

		auto& tfm = *obj->Transform();
		if (message.use_position)
			tfm.position = message.position;
		if (message.use_rotation)
			for (unsigned i = 0; i < 4; ++i)
				tfm.rotation[i] = message.rotation[i];

		ev->Setup();
	}

	void EventManager::OnTransferOwnershipEvent(EventTransferOwnershipMessage& message)
	{
		auto id = std::make_shared<SignalBase::SlotId>();
		LOG_TO(LogPool::NETWORK, "Received transfer ownership event");
		auto network_id = message.object_to_transfer;
		auto ev = Core::GetSystem<NetworkSystem>().GetIDManager().GetViewFromId(network_id);
		ev->owner = Core::GetSystem<NetworkSystem>().GetMe();
		ev->GetGameObject()->GetComponent<ElectronView>()->SetAsClientObject();
	}

	void EventManager::OnDestroyObjectMessage(EventDestroyObjectMessage& message)
	{
		auto view = Core::GetSystem<NetworkSystem>().GetIDManager().GetViewFromId(message.id);
		Core::GetGameState().DestroyObject(view->GetGameObject());
	}

	void EventManager::OnLoadLevelMessage(EventLoadLevelMessage& msg)
	{
		Core::GetSystem<SceneManager>().SetNextScene(msg.GetScene());
		vector<EventLoadLevelMessage::ViewMapping> views( msg.GetObjects().begin(), msg.GetObjects().end() );

		Core::GetSystem<SceneManager>().OnSceneChange.Listen(
			[&id_manager = Core::GetSystem<NetworkSystem>().GetIDManager(),
			views = std::move(views)]
		(RscHandle<Scene>)
		{
			for (auto [eview, view] : zip(Core::GetGameState().GetObjectsOfType<ElectronView>(), views))
			{
				id_manager.EmplaceID(view.id, eview.GetHandle());
				eview.Setup();
			}
		}, 1);
	}
}
