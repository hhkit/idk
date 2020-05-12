#include "stdafx.h"
#include "pch_common.h"
#include <yojimbo/yojimbo.h>

#include "NetworkSystem.h"

#include <app/Application.h>
#include <network/Server.h>
#include <network/Client.h>
#include <network/ServerConnectionManager.h>
#include <network/ClientConnectionManager.h>
#include <network/ClientMoveManager.h>
#include <network/ServerMoveManager.h>
#include <network/ConnectionManager.inl>
#include <network/IDManager.h>
#include <network/ElectronView.h>
#include <network/EventDataBlockFrameNumber.h>
#include <network/EventManager.h>
#include <network/GhostManager.h>
#include <core/GameState.h>
#include <core/Scheduler.h>
#include <core/GameObject.inl>
#include <script/MonoBehavior.h>
#include <script/ScriptSystem.h>
#include <phys/PhysicsSystem.h>
#include <thread>

namespace idk
{
	NetworkSystem::NetworkSystem() = default;
	NetworkSystem::~NetworkSystem() = default;

	void NetworkSystem::InstantiateServer(const Address& d)
	{
		ResetNetwork();
		frame_counter = SeqNo{};
		my_id = Host::SERVER;
		lobby = std::make_unique<Server>(Address{d.a,d.b,d.c,d.d, server_listen_port});
		lobby->OnClientConnect += [this](int clientid)
		{
			server_connection_manager[clientid] = std::make_unique<ServerConnectionManager>(clientid, *lobby);
			server_connection_manager[clientid]->CreateAndSendMessage<EventDataBlockFrameNumber>(GameChannel::RELIABLE, [&](EventDataBlockFrameNumber& msg)
				{
					msg.frame_count = frame_counter;
					msg.player_id = static_cast<Host>(clientid);
				});
			server_connection_manager[clientid]->GetManager<EventManager>()->SendBufferedEvents();
		};
		lobby->OnClientDisconnect += [this](int clientid)
		{
			server_connection_manager[clientid].reset();
		};
		id_manager = std::make_unique<IDManager>();
		EventManager::Init();
	}

	void NetworkSystem::ConnectToServer(const Address& d)
	{
		ResetNetwork();
		frame_counter = SeqNo{};
		client = std::make_unique<Client>(Address{ d.a,d.b,d.c,d.d, server_listen_port });
		client->OnConnectionToServer += [this]()
		{
			client_connection_manager = std::make_unique<ClientConnectionManager>(*client);
			client_connection_manager->Subscribe<EventDataBlockFrameNumber>([this](EventDataBlockFrameNumber& event)
				{
					frame_counter = event.frame_count;
					my_id = event.player_id;
				});
		};

		client->OnDisconnectionFromServer += [this]()
		{
			my_id = Host::NONE;
			client_connection_manager.reset();
		};
		id_manager = std::make_unique<IDManager>();
	}

	void NetworkSystem::Disconnect()
	{
		ResetNetwork();
	}

	array<ConnectionManager*, 5> NetworkSystem::GetConnectionManagers()
	{
		return array<ConnectionManager*, 5>{
			client_connection_manager.get(),
				server_connection_manager[0].get(),
				server_connection_manager[1].get(),
				server_connection_manager[2].get(),
				server_connection_manager[3].get(),
		};
	}

	bool NetworkSystem::IsHost()
	{
		return static_cast<bool>(lobby);
	}

	Host NetworkSystem::GetMe()
	{
		return my_id;
	}

	SeqNo NetworkSystem::GetSequenceNumber() const
	{
		return frame_counter;
	}

	ConnectionManager* NetworkSystem::GetConnectionTo(Host host)
	{
		switch (host)
		{
		case Host::SERVER:  return client_connection_manager.get();
		case Host::CLIENT0: return server_connection_manager[0].get();
		case Host::CLIENT1: return server_connection_manager[1].get();
		case Host::CLIENT2: return server_connection_manager[2].get();
		case Host::CLIENT3: return server_connection_manager[3].get();
		case Host::ANY:
		{
			if (client_connection_manager)
				return client_connection_manager.get();
			for (auto& elem : server_connection_manager)
				if (elem)
					return elem.get();
		}
		}
		return nullptr; // no connection found
	}

	void NetworkSystem::SetPacketLoss(float percent)
	{
		if (lobby)
			lobby->SetPacketLoss(percent);

		if (client)
			client->SetPacketLoss(percent);
	}

	void NetworkSystem::SetLatency(seconds time)
	{
		if (lobby)
			lobby->SetLatency(time);

		if (client)
			client->SetLatency(time);
	}

	void NetworkSystem::ReceivePackets()
	{
		if (lobby || client)
		{
			++frame_counter;
		}

		if (lobby)
		{
			lobby->ReceivePackets();
		}
		if (client)
		{
			client->ReceivePackets();
		}
	}

	void NetworkSystem::SendPackets()
	{
		if (lobby)
			lobby->SendPackets();

		if (client)
			client->SendPackets();
	}

	void NetworkSystem::Rollback(span<ElectronView> evs)
	{
		auto& physics_system = Core::GetSystem<PhysicsSystem>();
		auto& script_system  = Core::GetSystem<mono::ScriptSystem>();

		auto reserialize_thunk = std::get<mono::ManagedThunk>(script_system.Environment().Type("ElectronNetwork")->GetMethod("Reserialize", 1));

		for (auto& ev : evs)
		{
			if (auto client_inputs = std::get_if<ElectronView::ClientSideInputs>(&ev.move_state))
			{
				if (client_inputs->dirty_control_object == false)
					continue;

				for (auto& move : client_inputs->moves)
				{
					auto array = mono_array_new(mono_domain_get(), mono_get_byte_class(), move.payload.size());
					for (unsigned i = 0; i < move.payload.size(); ++i)
						mono_array_set(array, unsigned char, i, move.payload[i]);

					auto input = reserialize_thunk.Invoke((MonoObject*)array);
					
					// execute input
					for (auto& behavior : ev.GetGameObject()->GetComponents<mono::Behavior>())
					{
						auto& obj = behavior->GetObject();
						auto& type = *obj.Type();
						auto method = type.GetMethod("ProcessInput", 1);
						if (auto thunk = std::get_if<mono::ManagedThunk>(&method))
						{
							LOG_TO(LogPool::NETWORK, "Rollback Input Tick %d", move.index.value);
							thunk->Invoke(obj, input);
							break;
						}
					}

					// simulate physics
					if (auto rb = ev.GetGameObject()->GetComponent<RigidBody>())
						physics_system.SimulateOneObject(rb);
				}
			}

			if (auto server_inputs = std::get_if<ElectronView::ServerSideInputs>(&ev.move_state))
			{
				auto base = server_inputs->moves.base();
				if (auto move = server_inputs->moves.pop_front())
				{
					auto array = mono_array_new(mono_domain_get(), mono_get_byte_class(), move->size());
					for (unsigned i = 0; i < move->size(); ++i)
						mono_array_set(array, unsigned char, i, (*move)[i]);

					auto input = reserialize_thunk.Invoke((MonoObject*)array);

					// execute input
					for (auto& behavior : ev.GetGameObject()->GetComponents<mono::Behavior>())
					{
						auto& obj = behavior->GetObject();
						auto& type = *obj.Type();
						auto method = type.GetMethod("ProcessInput", 1);
						if (auto thunk = std::get_if<mono::ManagedThunk>(&method))
						{
							LOG_TO(LogPool::NETWORK, "Processed Move %d", base.value);
							thunk->Invoke(obj, input);
						}
					}
				}
			}
		}
	}

	void NetworkSystem::CollectInputs(span<ElectronView> evs)
	{
		auto& physics_system = Core::GetSystem<PhysicsSystem>();
		auto& script_system = Core::GetSystem<mono::ScriptSystem>();

		auto serialize_thunk = std::get<mono::ManagedThunk>(script_system.Environment().Type("ElectronNetwork")->GetMethod("Serialize", 1));
		const auto curr_frameid = GetSequenceNumber();

		for (auto& ev : evs)
		{
			if (auto client_inputs = std::get_if<ElectronView::ClientSideInputs>(&ev.move_state))
			{
				// execute input
				for (auto& behavior : ev.GetGameObject()->GetComponents<mono::Behavior>())
				{
					auto& obj = behavior->GetObject();
					auto& type = *obj.Type();
					auto method = type.GetMethod("GenerateInput");
					if (auto thunk = std::get_if<mono::ManagedThunk>(&method))
					{

						auto input = thunk->Invoke(obj);

						auto array = (MonoArray*) serialize_thunk.Invoke((MonoObject*) input);

						std::string payload;
						//payload.insert(payload.end(), array, (char*)array + mono_array_length(array));
						payload.resize(mono_array_length(array));

						for (unsigned i = 0; i < payload.size(); ++i)
							payload[i] = mono_array_get(array, char, i);

						client_inputs->moves.emplace_back(ElectronView::ClientSideInputs::MoveNode{ client_inputs->next_move_index, payload });
						client_inputs->next_move_index++;
						break;
					}
				}
			}
		}
	}

	void NetworkSystem::MoveGhosts(span<ElectronView> electron_views)
	{
		for (auto& ev : electron_views)
		{
			ev.MoveGhost(Core::GetScheduler().GetRealDeltaTime());
		}
	}

	void NetworkSystem::PreparePackets(span<ElectronView> electron_views)
	{
		for (auto& ev : electron_views)
		{
			ev.PrepareDataForSending(frame_counter);
		}
		// if server
		for (const auto& elem : server_connection_manager)
		{
			if (!elem)
				continue;

			elem->GetManager<GhostManager>()->SendGhosts(electron_views);
			elem->GetManager<ServerMoveManager>()->SendControlObjects(electron_views);
		}

		// if client
		if (client_connection_manager)
			client_connection_manager->GetManager<ClientMoveManager>()->SendMoves(electron_views);

		for (auto& ev : electron_views)
			ev.CacheSentData();
	}

	void NetworkSystem::AddCallbackTarget(Handle<mono::Behavior> behavior)
	{
		callback_objects.emplace_back(behavior);
	}

	void NetworkSystem::RemoveCallbackTarget(Handle<mono::Behavior> behavior)
	{
		callback_objects.erase(std::remove(callback_objects.begin(), callback_objects.end(), behavior), callback_objects.end());
	}

	span<const Handle<mono::Behavior>> NetworkSystem::GetCallbackTargets() const
	{
		return callback_objects;
	}


	void NetworkSystem::Init()
	{
		InitializeYojimbo();
	}

	void NetworkSystem::LateInit()
	{
		for (auto& device : Core::GetSystem<Application>().GetNetworkDevices())
		{
			LOG_TO(LogPool::NETWORK, "Found %s device (%s) with address %s.", device.name.c_str(), device.description.c_str(), string{ device.ip_addresses[0] }.c_str());
		}
	}

	void NetworkSystem::EarlyShutdown()
	{
		ResetNetwork();
	}

	void NetworkSystem::Shutdown()
	{
		ShutdownYojimbo();
	}
	void NetworkSystem::ResetNetwork()
	{
		my_id = Host::NONE;
		for (auto& elem : server_connection_manager)
			elem.reset();
		lobby.reset();
		client_connection_manager.reset();
		client.reset();
		id_manager.reset();

		// network ids no longer relevant
		for (auto& elem : Core::GetGameState().GetObjectsOfType<ElectronView>())
			elem.network_id = 0;
	}
}