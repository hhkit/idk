#include "stdafx.h"
#include <algorithm>

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
#include <network/EventDataBlockBufferedEvents.h>
#include <network/EventInstantiatePrefabMessage.h>
#include <network/EventTransferOwnershipMessage.h>
#include <network/EventInvokeRPCMessage.h>

namespace idk
{
	struct EventManager::BufferedEvents
	{
		struct InstantiatedPrefab
		{
			RscHandle<Prefab> prefab;
			Handle<ElectronView> obj;
		};

		RscHandle<Scene> loaded_scene;
		vector<Handle<GameObject>> loaded_views;
		vector<InstantiatedPrefab> loaded_prefabs;
	};

	unique_ptr<EventManager::BufferedEvents> buffered_events;

	void EventManager::Init()
	{
		buffered_events = std::make_unique<EventManager::BufferedEvents>();
	}

	void EventManager::ResetEventBuffer()
	{
		buffered_events.reset();
	}

	void EventManager::SubscribeEvents(ClientConnectionManager& client)
	{
		client.Subscribe<TestMessage>([](TestMessage& message) { LOG_TO(LogPool::NETWORK, "Received message %d", message.i); });
		client.Subscribe<EventInstantiatePrefabMessage>([this](EventInstantiatePrefabMessage& msg) { OnInstantiatePrefabEvent(msg); });
		client.Subscribe<EventTransferOwnershipMessage>([this](EventTransferOwnershipMessage& msg) { OnTransferOwnershipEvent(msg); });
		client.Subscribe<EventLoadLevelMessage>([this](EventLoadLevelMessage& msg) { OnLoadLevelMessage(msg); });
		client.Subscribe<EventInvokeRPCMessage>([this](EventInvokeRPCMessage& msg) { OnInvokeRPCMessage(msg); });
		client.Subscribe<EventDestroyObjectMessage>([this](EventDestroyObjectMessage& msg) {OnDestroyObjectMessage(msg); });
		client.Subscribe<EventDataBlockBufferedEvents>([this](EventDataBlockBufferedEvents& msg) {OnBufferedEventMessage(msg); });
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
	}

	void EventManager::SendTestMessage(int i)
	{
		LOG_TO(LogPool::NETWORK, "Sending test message w payload %d", i);
		Core::GetSystem<NetworkSystem>().BroadcastMessage <TestMessage>(GameChannel::RELIABLE, [&](TestMessage& msg)
			{
				msg.i = i;
			});
	}

	void EventManager::SendBufferedEvents()
	{
		if (buffered_events)
		{
			connection_manager->CreateAndSendMessage<EventDataBlockBufferedEvents>(GameChannel::RELIABLE, [](EventDataBlockBufferedEvents& msg)
				{
					msg.load_scene = buffered_events->loaded_scene;
					for (auto& elem : buffered_events->loaded_views)
					{
						if (!elem)
						{
							auto& obj = msg.loaded_objects.emplace_back();
							obj.handle = elem;
							obj.destroyed = true;
						}
						else
						{
							auto& obj = msg.loaded_objects.emplace_back();
							obj.handle = elem;
							obj.id = elem->GetComponent<ElectronView>()->network_id;
							obj.position = elem->Transform()->position;
							obj.rotation = elem->Transform()->rotation;
						}
					}

					for (auto& elem : buffered_events->loaded_prefabs)
					{
						auto& obj_and_pfb = msg.loaded_prefabs.emplace_back();
						obj_and_pfb.prefab = elem.prefab;
						obj_and_pfb.id = elem.obj->network_id;
						obj_and_pfb.position = elem.obj->GetGameObject()->Transform()->position;
						obj_and_pfb.rotation = elem.obj->GetGameObject()->Transform()->rotation;
					}
				});
		}
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
		
		buffered_events->loaded_prefabs.emplace_back(EventManager::BufferedEvents::InstantiatedPrefab{ prefab, ev });

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
		scene_changing = true;
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

			buffered_events = std::make_unique<EventManager::BufferedEvents>();
			buffered_events->loaded_scene = scene;
			for (auto& elem : Core::GetGameState().GetObjectsOfType<ElectronView>())
				buffered_events->loaded_views.emplace_back(elem.GetGameObject());

			Core::GetSystem<NetworkSystem>().BroadcastMessage<EventLoadLevelMessage>(GameChannel::RELIABLE,
				[scene](EventLoadLevelMessage& msg)
				{
					msg.SetScene(scene);
					for (auto& elem : Core::GetGameState().GetObjectsOfType<ElectronView>())
						msg.AddView(elem.GetHandle());
				});

			scene_changing = false;
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

		if (buffered_events)
		{
			buffered_events->loaded_prefabs.erase(
				std::remove_if(buffered_events->loaded_prefabs.begin(), buffered_events->loaded_prefabs.end(), [&](auto& elem) -> bool
					{
						return elem.obj == view;
					}),
				buffered_events->loaded_prefabs.end()
			);
		}

		Core::GetGameState().DestroyObject(view->GetGameObject());
	}

	// targets
	void EventManager::OnInstantiatePrefabEvent(EventInstantiatePrefabMessage& message)
	{
		if (!message.prefab)
		{
			LOG_TO(LogPool::NETWORK, "Prefab %s does not exist", static_cast<string>(message.prefab.guid).c_str());
			return;
		}


		const auto instantiate_prefab = 
		[	prefab = message.prefab,
			id = message.id,
			use_position = message.use_position, 
			position = message.position,
			use_rotation = message.use_rotation, 
			rotation = quat{ message.rotation[0],message.rotation[1],message.rotation[2],message.rotation[3] }
		] (RscHandle<Scene>)
		{

			auto obj = prefab->Instantiate(*Core::GetSystem<SceneManager>().GetActiveScene());
			auto ev = obj->GetComponent<ElectronView>();

			// set to ghost state
			ev->ghost_state = ElectronView::Ghost{};

			IDK_ASSERT(Core::GetSystem<NetworkSystem>().GetIDManager().EmplaceID(id, ev));

			auto& tfm = *obj->Transform();
			if (use_position)
				tfm.position = position;

			if (use_rotation)
				for (unsigned i = 0; i < 4; ++i)
					tfm.rotation[i] = rotation[i];

			ev->Setup();
		};

		if (scene_changing)
			Core::GetSystem<SceneManager>().OnSceneChange.Listen(instantiate_prefab, 1);
		else
			instantiate_prefab({});
	}

	void EventManager::OnTransferOwnershipEvent(EventTransferOwnershipMessage& message)
	{
		const auto transfer_ownership = [network_id = message.object_to_transfer](RscHandle<Scene>)
		{
			auto ev = Core::GetSystem<NetworkSystem>().GetIDManager().GetViewFromId(network_id);
			ev->owner = Core::GetSystem<NetworkSystem>().GetMe();
			ev->GetGameObject()->GetComponent<ElectronView>()->SetAsClientObject();
		};

		if (scene_changing)
			Core::GetSystem<SceneManager>().OnSceneChange.Listen(transfer_ownership, 1);
		else
			transfer_ownership({});

	}

	void EventManager::OnDestroyObjectMessage(EventDestroyObjectMessage& message)
	{
		const auto destroy_object =
			[id = message.id](RscHandle<Scene>)
		{
			auto view = Core::GetSystem<NetworkSystem>().GetIDManager().GetViewFromId(id);
			Core::GetGameState().DestroyObject(view->GetGameObject());
		};

		if (scene_changing)
			Core::GetSystem<SceneManager>().OnSceneChange.Listen(destroy_object, 1);
		else
			destroy_object({});
	}

	void EventManager::OnLoadLevelMessage(EventLoadLevelMessage& msg)
	{
		scene_changing = true;
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
			scene_changing = false;
		}, 1);
	}

	void EventManager::OnBufferedEventMessage(EventDataBlockBufferedEvents& msg)
	{
		auto create_prefabs = [this, prefabs = msg.loaded_prefabs]()
		{
			for (auto& prefab : prefabs)
			{
				EventInstantiatePrefabMessage prefab_msg;
				prefab_msg.prefab = prefab.prefab;
				prefab_msg.id = prefab.id;
				prefab_msg.use_position = true;
				prefab_msg.position = prefab.position;
				prefab_msg.use_rotation = true;
				for (auto i = 0; i < 4; ++i)
					prefab_msg.rotation[i] = prefab.rotation[i];

				OnInstantiatePrefabEvent(prefab_msg);
			}
		};

		if (msg.load_scene)
		{
			Core::GetSystem<SceneManager>().SetNextScene(msg.load_scene);

			Core::GetSystem<SceneManager>().OnSceneChange.Listen(
				[this,
				&id_manager = Core::GetSystem<NetworkSystem>().GetIDManager(),
				objects = msg.loaded_objects,
				prefabs = msg.loaded_prefabs,
				create_prefabs
				](RscHandle<Scene>)
			{
				for (auto& obj : objects)
				{
					if (obj.destroyed)
					{
						Core::GetGameState().DestroyObject(obj.handle);
					}
					else
					{
						LOG_TO(LogPool::NETWORK, "moving obj %ld to (%f,%f,%f)", obj.handle.id, obj.position.x, obj.position.y, obj.position.z);
						obj.handle->Transform()->position = obj.position;
						obj.handle->Transform()->rotation = obj.rotation;
						auto eview = obj.handle->GetComponent<ElectronView>();
						id_manager.EmplaceID(obj.id, eview);
						eview->ghost_state = ElectronView::Ghost{};
						eview->Setup();
					}
				}

				create_prefabs();
			}
			, 1);
		}
		else
			create_prefabs();
	}
}
