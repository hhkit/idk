#include "stdafx.h"
#include "pch_common.h"

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
#include <gfx/DebugRenderer.h>
#include <core/GameObject.h>
#include <common/Transform.h>
#include <phys/RigidBody.h>
#include <scene/SceneManager.h>
#include <scene/SceneGraph.inl>

#include <steam/steam_api_common.h>
#include <steam/isteamuser.h>
#include <steam/steamnetworkingtypes.h>

#include <algorithm>
#include <iterator>

#include <network/TestMessage.h>

//#pragma optimize("", off)

namespace idk
{
	NetworkSystem::NetworkSystem() = default;
	NetworkSystem::~NetworkSystem() = default;

	void NetworkSystem::CreateLobby(ELobbyType lobby_type)
	{
		ResetNetwork();
		frame_counter = SeqNo{};

		SteamMatchmaking()->CreateLobby(lobby_type, GameConfiguration::MAX_LOBBY_MEMBERS);

		SteamNetworkingUtils()->InitRelayNetworkAccess();
	}

	void NetworkSystem::OnLobbyCreated(LobbyCreated_t* callback)
	{
		if (callback->m_eResult == k_EResultOK)
		{
			lobby_id = callback->m_ulSteamIDLobby;
			my_id = Host::SERVER;
			lobby_members[0].id = SteamUser()->GetSteamID();
			lobby_members[0].host = my_id;

			LOG_TO(LogPool::NETWORK, "Lobby Steam ID is %llu", lobby_id.ConvertToUint64());

			server = std::make_unique<Server>(lobby_id);
			id_manager = std::make_unique<IDManager>();
			EventManager::Init();

			listen_socket = SteamNetworkingSockets()->CreateListenSocketP2P(0, 0, nullptr);

			for (auto& target : callback_objects)
				target->FireMessage("OnLobbyCreated", true);
		}
		else
		{
			LOG_CRASH_TO(LogPool::NETWORK, "Could not create server! EResult Code: %d", callback->m_eResult);

			for (auto& target : callback_objects)
				target->FireMessage("OnLobbyCreated", false);
		}
	}

	void NetworkSystem::JoinLobby(CSteamID _lobby_id)
	{
		ResetNetwork();
		frame_counter = SeqNo{};

		SteamMatchmaking()->JoinLobby(_lobby_id);

		SteamNetworkingUtils()->InitRelayNetworkAccess();
	}

	void NetworkSystem::OnJoinedLobby(LobbyEnter_t* callback)
	{
		if (callback->m_EChatRoomEnterResponse == k_EChatRoomEnterResponseSuccess)
		{
			if (my_id == Host::NONE)
			{
				lobby_id = callback->m_ulSteamIDLobby;
				client = std::make_unique<Client>(lobby_id);
				id_manager = std::make_unique<IDManager>();

				// obtain preexisting lobby members
				int num = SteamMatchmaking()->GetNumLobbyMembers(lobby_id);
				for (int i = 0; i < num; ++i)
				{
					CSteamID id = SteamMatchmaking()->GetLobbyMemberByIndex(lobby_id, i);
					const char* info = SteamMatchmaking()->GetLobbyMemberData(lobby_id, id, "info");
					int index = info[0] - '0';
					Host host = static_cast<Host>(info[1] - '0');

					lobby_members[index].id = id;
					lobby_members[index].host = host;
				}

				// find appropriate host
				Host host = Host::CLIENT0;
				int index = -1;
				for (int i = 0; i < GameConfiguration::MAX_LOBBY_MEMBERS; ++i)
				{
					if (lobby_members[i].id.IsValid() && lobby_members[i].host == host)
					{
						host = static_cast<Host>(static_cast<int>(host) + 1);
						i = -1;
						continue;
					}
					if (index == -1 && !lobby_members[i].id.IsValid())
						index = i;
				}
				if (index >= 0)
				{
					lobby_members[index].id = SteamUser()->GetSteamID();
					lobby_members[index].host = my_id = host;
				}
			}
			int my_index = -1;
			for (my_index = 0; my_index < GameConfiguration::MAX_LOBBY_MEMBERS; ++my_index)
			{
				if (lobby_members[my_index].host == my_id)
					break;
			}

			// on join, store my lobby member index and my host id
			char buf[3] { 0, 0, 0 };
			buf[0] = '0' + static_cast<char>(my_index);
			buf[1] = '0' + static_cast<char>(my_id);
			SteamMatchmaking()->SetLobbyMemberData(lobby_id, "info", buf);

			for (auto& target : callback_objects)
				target->FireMessage("OnLobbyJoined", true);
		}
		else
		{
			LOG_CRASH_TO(LogPool::NETWORK, "Could not connect to lobby! EChatRoomEnterResponse Code: %d", callback->m_EChatRoomEnterResponse);
			for (auto& target : callback_objects)
				target->FireMessage("OnLobbyJoined", false);
		}
	}

	// other members join or leave
	void NetworkSystem::OnLobbyChatUpdated(LobbyChatUpdate_t* callback)
	{
		if (callback->m_rgfChatMemberStateChange & k_EChatMemberStateChangeEntered)
		{
			CSteamID id = callback->m_ulSteamIDUserChanged;

			// find appropriate host
			Host host = Host::CLIENT0;
			int index = -1;
			for (int i = 0; i < GameConfiguration::MAX_LOBBY_MEMBERS; ++i)
			{
				if (lobby_members[i].id.IsValid() && lobby_members[i].host == host)
				{
					host = static_cast<Host>(static_cast<int>(host) + 1);
					i = -1;
					continue;
				}
				if (index == -1 && !lobby_members[i].id.IsValid())
					index = i;
			}

			if (index >= 0)
			{
				lobby_members[index].id = id;
				lobby_members[index].host = host;

				for (auto& target : callback_objects)
				{
					auto player_type = Core::GetSystem<mono::ScriptSystem>().Environment().Type("Client");
					auto player = player_type->ConstructTemporary(host);

					target->FireMessage("OnLobbyMemberJoined", player);
				}
			}
		}
		else if (callback->m_rgfChatMemberStateChange & ~k_EChatMemberStateChangeEntered)
		{
			CSteamID id = callback->m_ulSteamIDUserChanged;
			for (int i = 0; i < GameConfiguration::MAX_LOBBY_MEMBERS; ++i)
			{
				if (lobby_members[i].id == id)
				{
					lobby_members[i].id = k_steamIDNil;

					auto player_type = Core::GetSystem<mono::ScriptSystem>().Environment().Type("Client");
					auto player = player_type->ConstructTemporary(lobby_members[i].host);
					for (auto& target : callback_objects)
						target->FireMessage("OnLobbyMemberLeft", player);
					break;
				}
			}
		}
	}

	void NetworkSystem::LeaveLobby()
	{
		if (IsHost())
			SteamMatchmaking()->LeaveLobby(lobby_id);

		ResetNetwork();
		frame_counter = SeqNo{};
	}

	void NetworkSystem::FindLobbies()
	{
		SteamMatchmaking()->RequestLobbyList();
	}

	CSteamID NetworkSystem::GetLobbyMember(Host host)
	{
		for (auto& elem : lobby_members)
		{
			if (elem.host == host)
				return elem.id;
		}
		return k_steamIDNil;
	}

	int NetworkSystem::GetLobbyMemberIndex(Host host)
	{
		for (int i = 0; i < GameConfiguration::MAX_LOBBY_MEMBERS; ++i)
		{
			if (lobby_members[i].host == host)
				return i;
		}
		return -1;
	}

	void NetworkSystem::ConnectToLobbyOwner()
	{
		if (IsHost())
			return;

		auto owner = SteamMatchmaking()->GetLobbyOwner(lobby_id);
		if (owner == k_steamIDNil)
		{
			LOG_CRASH_TO(LogPool::NETWORK, "Cannot connect to invalid lobby or lobby you're not a member of!");
			return;
		}

		SteamNetworkingIdentity identity;
		identity.SetSteamID(owner);
		SteamNetworkingSockets()->ConnectP2P(identity, 0, 0, nullptr);
	}

	void NetworkSystem::OnConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* callback)
	{
		/// - A new connection arrives on a listen socket.
		///   m_info.m_hListenSocket will be set, m_eOldState = k_ESteamNetworkingConnectionState_None,
		///   and m_info.m_eState = k_ESteamNetworkingConnectionState_Connecting.
		///   See ISteamNetworkigSockets::AcceptConnection.
		if (callback->m_eOldState == k_ESteamNetworkingConnectionState_None
			&& callback->m_info.m_eState == k_ESteamNetworkingConnectionState_Connecting
			&& callback->m_info.m_hListenSocket != k_HSteamListenSocket_Invalid)
		{
			auto res = SteamNetworkingSockets()->AcceptConnection(callback->m_hConn);
			if (res == k_EResultInvalidState || res == k_EResultInvalidParam)
				LOG_CRASH_TO(LogPool::NETWORK, "Could not connect to client P2P!");
		}
		/// - A connection you initiated has been accepted by the remote host.
		///   m_eOldState = k_ESteamNetworkingConnectionState_Connecting, and
		///   m_info.m_eState = k_ESteamNetworkingConnectionState_Connected.
		///   Some connections might transition to k_ESteamNetworkingConnectionState_FindingRoute first.
		else if (callback->m_eOldState == k_ESteamNetworkingConnectionState_Connecting
				 && (callback->m_info.m_eState == k_ESteamNetworkingConnectionState_Connected
				 	 || callback->m_info.m_eState == k_ESteamNetworkingConnectionState_FindingRoute))
		{
			if (client)
			{
				client_connection_manager = std::make_unique<ClientConnectionManager>(*client, callback->m_hConn);
					client_connection_manager->Subscribe<EventDataBlockFrameNumber>([this](EventDataBlockFrameNumber& event)
				{
					frame_counter = event.frame_count;
					LOG_TO(LogPool::NETWORK, "Receiving frame number: %u", frame_counter.value);
				});

					LOG_TO(LogPool::NETWORK, "Connected to server");
				for (auto& target : callback_objects)
					target->FireMessage("OnConnectedToServer");
			}
			else if (server)
			{
				CSteamID id = callback->m_info.m_identityRemote.GetSteamID();

				for (size_t i = 0; i < GameConfiguration::MAX_LOBBY_MEMBERS; ++i)
				{
					if (lobby_members[i].id != id)
						continue;

					int clientIndex = static_cast<int>(lobby_members[i].host);

					server_connection_manager[clientIndex] = std::make_unique<ServerConnectionManager>(clientIndex, *server, callback->m_hConn);
					server_connection_manager[clientIndex]->CreateAndSendMessage<EventDataBlockFrameNumber>(GameChannel::RELIABLE, [&](EventDataBlockFrameNumber& msg)
					{
						msg.frame_count = frame_counter;
						LOG_TO(LogPool::NETWORK, "Sending frame number: %u", frame_counter.value);
					});
					server_connection_manager[clientIndex]->GetManager<EventManager>()->SendBufferedEvents();

					LOG_TO(LogPool::NETWORK, "Client %d connected", clientIndex);
					auto player_type = Core::GetSystem<mono::ScriptSystem>().Environment().Type("Client");
					auto player = player_type->ConstructTemporary(clientIndex);
					for (auto& target : callback_objects)
						target->FireMessage("OnClientConnected", player);

					break;
				}
			}
		}
		/// - A connection has been actively rejected or closed by the remote host.
		///   m_eOldState = k_ESteamNetworkingConnectionState_Connecting or k_ESteamNetworkingConnectionState_Connected,
		///   and m_info.m_eState = k_ESteamNetworkingConnectionState_ClosedByPeer.  m_info.m_eEndReason
		///   and m_info.m_szEndDebug will have for more details.
		///   NOTE: upon receiving this callback, you must still destroy the connection using
		///   ISteamNetworkingSockets::CloseConnection to free up local resources.  (The details
		///   passed to the function are not used in this case, since the connection is already closed.)
		else if ((callback->m_eOldState == k_ESteamNetworkingConnectionState_Connecting
				  || callback->m_eOldState == k_ESteamNetworkingConnectionState_Connected)
				 && callback->m_info.m_eState == k_ESteamNetworkingConnectionState_ClosedByPeer)
		{
			if (client)
			{
				LOG_TO(LogPool::NETWORK, "Disconnected from server");
				for (auto& target : callback_objects)
					target->FireMessage("OnDisconnectedFromServer");

				ResetNetwork();
			}
			else if (server)
			{
				for (size_t i = 0; i < GameConfiguration::MAX_CLIENTS; ++i)
				{
					if (!server_connection_manager[i] || server_connection_manager[i]->GetHandle() != callback->m_hConn)
						continue;

					LOG_TO(LogPool::NETWORK, "Client %d disconnected", i);
					auto player_type = Core::GetSystem<mono::ScriptSystem>().Environment().Type("Client");
					auto player = player_type->ConstructTemporary(i);
					for (auto& target : callback_objects)
						target->FireMessage("OnClientDisconnected", player);

					server_connection_manager[i].reset();
					for (auto& elem : lobby_members)
					{
						if (elem.host == static_cast<Host>(i))
						{
							elem.host = Host::NONE;
							elem.id = k_steamIDNil;
							break;
						}
					}
					break;
				}
			}
		}
		/// - A problem was detected with the connection, and it has been closed by the local host.
		///   The most common failure is timeout, but other configuration or authentication failures
		///   can cause this.  m_eOldState = k_ESteamNetworkingConnectionState_Connecting or
		///   k_ESteamNetworkingConnectionState_Connected, and m_info.m_eState = k_ESteamNetworkingConnectionState_ProblemDetectedLocally.
		///   m_info.m_eEndReason and m_info.m_szEndDebug will have for more details.
		///   NOTE: upon receiving this callback, you must still destroy the connection using
		///   ISteamNetworkingSockets::CloseConnection to free up local resources.  (The details
		///   passed to the function are not used in this case, since the connection is already closed.)
		else if ((callback->m_eOldState == k_ESteamNetworkingConnectionState_Connecting
				  || callback->m_eOldState == k_ESteamNetworkingConnectionState_Connected)
				 && callback->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally)
		{
			if (client)
			{
				LOG_TO(LogPool::NETWORK, "Disconnected from server");
				for (auto& target : callback_objects)
					target->FireMessage("OnDisconnectedFromServer");

				ResetNetwork();
			}
			else if (server)
			{
				for (size_t i = 0; i < GameConfiguration::MAX_CLIENTS; ++i)
				{
					if (!server_connection_manager[i] || server_connection_manager[i]->GetHandle() != callback->m_hConn)
						continue;

					LOG_TO(LogPool::NETWORK, "Client %d disconnected", i);
					auto player_type = Core::GetSystem<mono::ScriptSystem>().Environment().Type("Client");
					auto player = player_type->ConstructTemporary(i);
					for (auto& target : callback_objects)
						target->FireMessage("OnClientDisconnected", player);

					server_connection_manager[i].reset();
					for (auto& elem : lobby_members)
					{
						if (elem.host == static_cast<Host>(i))
						{
							elem.host = Host::NONE;
							elem.id = k_steamIDNil;
							break;
						}
					}
					break;
				}
			}
		}
	}

	void NetworkSystem::OnLobbyDataUpdated(LobbyDataUpdate_t* callback)
	{
		if (!callback->m_bSuccess)
			return;
		if (callback->m_ulSteamIDMember != lobby_id)
			return;

		for (auto& target : callback_objects)
			target->FireMessage("OnLobbyDataUpdated");
	}

	void NetworkSystem::OnLobbyMatchList(LobbyMatchList_t* callback)
	{
		auto lobby_type = Core::GetSystem<mono::ScriptSystem>().Environment().Type("Lobby");
		auto* arr = mono_array_new(mono_domain_get(), lobby_type->Raw(), callback->m_nLobbiesMatching);
		for (uint32 i = 0; i < callback->m_nLobbiesMatching; ++i)
		{
			auto lobby = lobby_type->ConstructTemporary(SteamMatchmaking()->GetLobbyByIndex(i).ConvertToUint64());
			mono_array_setref(arr, i, lobby);
		}

		for (auto& target : callback_objects)
			target->FireMessage("OnLobbyMatchList", arr);
	}

	void NetworkSystem::OnLobbyChatMsg(LobbyChatMsg_t* callback)
	{
		std::byte buf[128];
		int sz = SteamMatchmaking()->GetLobbyChatEntry(lobby_id, callback->m_iChatID, nullptr, buf, 128, nullptr);

		CSteamID id = callback->m_ulSteamIDUser;
		for (int i = 0; i < GameConfiguration::MAX_LOBBY_MEMBERS; ++i)
		{
			if (lobby_members[i].id == id)
			{
				MonoArray* arr = mono_array_new(mono_domain_get(), mono_get_byte_class(), sz);
				std::memcpy(mono_array_addr(arr, std::byte, 0), buf, sz);

				auto player_type = Core::GetSystem<mono::ScriptSystem>().Environment().Type("Client");
				auto player = player_type->ConstructTemporary(lobby_members[i].host);

				for (auto& target : callback_objects)
					target->FireMessage("OnLobbyChatMsg", player, arr);
				break;
			}
		}
	}

	void NetworkSystem::OnLobbyJoinRequested(GameLobbyJoinRequested_t* callback)
	{
		auto lobby_type = Core::GetSystem<mono::ScriptSystem>().Environment().Type("Lobby");
		auto lobby = lobby_type->ConstructTemporary(callback->m_steamIDLobby);

		for (auto& target : callback_objects)
			target->FireMessage("OnLobbyJoinRequested", lobby);
	}

	void NetworkSystem::Disconnect()
	{
		ResetNetwork();
	}

	bool NetworkSystem::IsHost()
	{
		return static_cast<bool>(server);
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
		case Host::SERVER: return client_connection_manager.get();
		case Host::CLIENT0: return server_connection_manager[0].get();
		case Host::CLIENT1: return server_connection_manager[1].get();
		case Host::CLIENT2: return server_connection_manager[2].get();
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
		if (server)
			server->SetPacketLoss(percent);

		if (client)
			client->SetPacketLoss(percent);
	}

	void NetworkSystem::SetLatency(seconds time)
	{
		if (server)
			server->SetLatency(time);

		if (client)
			client->SetLatency(time);
	}

	const static string_view discovery_message = "tian shou the millenial";

	void NetworkSystem::ReceivePackets()
	{
		if (server || client)
		{
			++frame_counter;
		}

		for (auto& manager : server_connection_manager)
		{
			if (manager)
				manager->ReceiveMessages();
		}
		if (client_connection_manager)
		{
			client_connection_manager->ReceiveMessages();
		}
	}

	void NetworkSystem::SendPackets()
	{
		if (server)
			server->SendPackets();

		if (client)
			client->SendPackets();

		if (connect_lobby_first_frame.IsValid())
		{
			GameLobbyJoinRequested_t callback;
			callback.m_steamIDLobby = connect_lobby_first_frame;
			connect_lobby_first_frame = k_steamIDNil;
			OnLobbyJoinRequested(&callback);
		}
	}

	void NetworkSystem::Rollback(span<ElectronView> evs)
	{
		auto& physics_system = Core::GetSystem<PhysicsSystem>();
		auto& script_system  = Core::GetSystem<mono::ScriptSystem>();

		auto reserialize_thunk = std::get<mono::ManagedThunk>(script_system.Environment().Type("ElectronNetwork")->GetMethod("Reserialize", 1));

		is_rolling_back = true;
		for (auto& ev : evs)
		{
			if (auto client_inputs = std::get_if<ElectronView::ClientSideInputs>(&ev.move_state))
			{
				if (client_inputs->dirty_control_object == false)
					continue;

				Core::GetSystem<DebugRenderer>().Draw(sphere{ ev.GetGameObject()->Transform()->position, 0.5f }, color{ 1,0,0 }, Core::GetDT());
				for (auto& move : client_inputs->moves)
				{
					auto array = mono_array_new(mono_domain_get(), mono_get_byte_class(), move.payload.size());
					for (unsigned i = 0; i < move.payload.size(); ++i)
						mono_array_set(array, unsigned char, i, move.payload[i]);

					auto input = reserialize_thunk.Invoke((MonoObject*)array);
					
					// execute input
					for (auto& comp : ev.observed_components)
					{
						if (comp.is_type<mono::Behavior>())
						{
							auto behavior = handle_cast<mono::Behavior>(comp);
							auto& obj = behavior->GetObject();
							auto& type = *obj.Type();
							auto method = type.GetMethod("ProcessInput", 1);
							if (auto thunk = std::get_if<mono::ManagedThunk>(&method))
							{
								// LOG_TO(LogPool::NETWORK, "Rollback Input Tick %d", move.index.value);
								thunk->Invoke(obj, input);
								break;
							}
						}
					}

					// simulate physics
					if (auto rb = ev.GetGameObject()->GetComponent<RigidBody>())
					{
						physics_system.SimulateOneObject(rb);
						// auto pos = ev.GetGameObject()->Transform()->position;
						// auto vel = rb->velocity();
						// LOG_TO(LogPool::NETWORK, "MOVE %d: POS[ %f,%f,%f ], VEL[%f, %f, %f]",
						// 	move.index, pos.x, pos.y, pos.z, vel.x, vel.y, vel.z);
					}

					Core::GetSystem<DebugRenderer>().Draw(sphere{ ev.GetGameObject()->Transform()->position, 0.5f }, 
						color{0,1,0}, Core::GetDT());

				}
				client_inputs->dirty_control_object = false;
			}

			if (auto server_inputs = std::get_if<ElectronView::ServerSideInputs>(&ev.move_state))
			{
				const auto base = server_inputs->moves.base();
				if (auto move = server_inputs->moves.pop_front())
				{
					auto array = mono_array_new(mono_domain_get(), mono_get_byte_class(), move->size());
					for (unsigned i = 0; i < move->size(); ++i)
						mono_array_set(array, unsigned char, i, (*move)[i]);

					auto input = reserialize_thunk.Invoke((MonoObject*)array);

					// execute input
					for (auto& comp : ev.observed_components)
					{
						if (comp.is_type<mono::Behavior>())
						{
							auto behavior = handle_cast<mono::Behavior>(comp);
							auto& obj = behavior->GetObject();
							auto& type = *obj.Type();
							auto method = type.GetMethod("ProcessInput", 1);
							if (const auto thunk = std::get_if<mono::ManagedThunk>(&method))
							{
								// LOG_TO(LogPool::NETWORK, "Processed Move %d", base.value);
								thunk->Invoke(obj, input);
							}
						}
					}
				}
			}
		}
		is_rolling_back = false;
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
				for (auto& comp : ev.observed_components)
				{
					if (comp.is_type<mono::Behavior>())
					{
						auto behavior = handle_cast<mono::Behavior>(comp);
						auto& obj = behavior->GetObject();
						auto& type = *obj.Type();
						auto method = type.GetMethod("GenerateInput");
						if (auto thunk = std::get_if<mono::ManagedThunk>(&method))
						{
							auto input = thunk->Invoke(obj);
							auto array = (MonoArray*)serialize_thunk.Invoke((MonoObject*)input);

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
	}

	void NetworkSystem::MoveGhosts(span<ElectronView> electron_views)
	{
		for (auto& ev : electron_views)
		{
			ev.MoveGhost(Core::GetScheduler().GetFixedDeltaTime(), prediction_weight);
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
		//yojimbo_log_level(YOJIMBO_LOG_LEVEL_ERROR);
		//yojimbo_set_printf_function([](const char* fmt, ...) -> int
		//{
		//	static char buf[1024];
		//	va_list args;
		//	va_start(args, fmt);
		//	vsprintf_s(buf, fmt, args);
		//	va_end(args);

		//	LOG_TO(LogPool::NETWORK, buf);

		//});
		//InitializeYojimbo();

	}

	void NetworkSystem::LateInit()
	{
	}

	void NetworkSystem::EarlyShutdown()
	{
		ResetNetwork();
	}

	void NetworkSystem::Shutdown()
	{
		//ShutdownYojimbo();
	}
	void NetworkSystem::ResetNetwork()
	{
		my_id = Host::NONE;
		for (auto& elem : server_connection_manager)
			elem.reset();
		server.reset();
		client_connection_manager.reset();
		client.reset();
		id_manager.reset();
		for (auto& elem : lobby_members)
		{
			elem.host = Host::NONE;
			elem.id = k_steamIDNil;
		}
		lobby_id = k_steamIDNil;

		// network ids no longer relevant
		for (auto& elem : Core::GetGameState().GetObjectsOfType<ElectronView>())
			elem.network_id = 0;
	}

};