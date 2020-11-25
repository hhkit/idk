#pragma once
#include <core/ISystem.h>
#include <network/GameConfiguration.h>
#include <event/Signal.h>
#include <network/network.h>
#include <network/Message.h>
#include <steam/steam_api_common.h>
#pragma warning(push)
#pragma warning(disable:4996)
#include <steam/isteammatchmaking.h>
#pragma warning(pop)
#include <steam/isteamnetworkingsockets.h>

namespace idk
{
	class Client;
	class Server;
	class ConnectionManager;
	class IDManager;
	class ClientConnectionManager;
	class ServerConnectionManager;
	class Socket;

	class NetworkSystem
		: public ISystem
	{
	public:
		NetworkSystem();
		~NetworkSystem();

		CSteamID GetLobbyID() { return lobby_id; }
		void CreateLobby(ELobbyType lobby_type);
		void JoinLobby(CSteamID lobby_id);
		void LeaveLobby();
		void SendLobbyMsg(const string& msg);
		void FindLobbies();
		CSteamID GetLobbyMember(Host host);
		int GetLobbyMemberIndex(Host host);
		void ConnectToLobbyOwner();

		void Disconnect();
		void EvictClient(int clientID);

		Client& GetClient() { return *client; }
		Server& GetServer() { return *server; }

		bool IsHost();
		Host GetMe();
		SeqNo GetSequenceNumber() const;
		ConnectionManager* GetConnectionTo(Host host = Host::ANY);
		template<typename Message, typename InstantiationFunc, typename = sfinae<std::is_invocable_v<InstantiationFunc, Message&>>>
		void BroadcastMessage(GameChannel channel, InstantiationFunc&& func);
		IDManager& GetIDManager() { return *id_manager; }

		void SetPacketLoss(float percent);
		void SetLatency(seconds time);

		void ReceivePackets();
		void SendPackets();

		void Rollback(span<ElectronView>);
		void CollectInputs(span<ElectronView>);

		void MoveGhosts(span<ElectronView>);
		void PreparePackets(span<ElectronView>);

		void AddCallbackTarget(Handle<mono::Behavior> behavior);
		void RemoveCallbackTarget(Handle<mono::Behavior> behavior);
		span<const Handle<mono::Behavior>> GetCallbackTargets() const;

		template<typename ... Objects> void SubscribePacketResponse(void(*fn)(span<Objects...>));

	private:
		struct ResponseCallback
		{
			void* fn_ptr;
			function<void()> callback;
		};
		struct LobbyMember
		{
			CSteamID id;
			Host host;
		};

		unique_ptr<Server> server;
		unique_ptr<Client> client;
		unique_ptr<ServerConnectionManager> server_connection_manager[GameConfiguration::MAX_CLIENTS];
		unique_ptr<ClientConnectionManager> client_connection_manager;

		SeqNo frame_counter{};
		Host my_id;
		CSteamID lobby_id;
		LobbyMember lobby_members[GameConfiguration::MAX_LOBBY_MEMBERS];

		unique_ptr<IDManager> id_manager;

		vector<ResponseCallback> frame_start_callbacks;
		vector<ResponseCallback> frame_end_callbacks;

		vector<Handle<mono::Behavior>> callback_objects;

		HSteamListenSocket listen_socket;



		void Init() override;
		void LateInit() override;
		void EarlyShutdown() override;
		void Shutdown() override;

		void ResetNetwork();



		STEAM_CALLBACK(NetworkSystem, OnLobbyCreated, LobbyCreated_t);
		STEAM_CALLBACK(NetworkSystem, OnJoinedLobby, LobbyEnter_t);
		STEAM_CALLBACK(NetworkSystem, OnConnectionStatusChanged, SteamNetConnectionStatusChangedCallback_t);
		STEAM_CALLBACK(NetworkSystem, OnLobbyChatUpdated, LobbyChatUpdate_t);
		STEAM_CALLBACK(NetworkSystem, OnLobbyDataUpdated, LobbyDataUpdate_t);
		STEAM_CALLBACK(NetworkSystem, OnLobbyMatchList, LobbyMatchList_t);
		STEAM_CALLBACK(NetworkSystem, OnLobbyChatMsg, LobbyChatMsg_t);

	};
}