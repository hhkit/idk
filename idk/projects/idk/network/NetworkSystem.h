#pragma once
#include <yojimbo/yojimbo.h>
#include <core/ISystem.h>
#include <network/Address.h>
#include <network/GameConfiguration.h>
#include <event/Signal.h>
#include <network/network.h>
namespace idk
{
	class Client;
	class Server;
	class ConnectionManager;
	class IDManager;
	class ClientConnectionManager;
	class ServerConnectionManager;
	class Socket;

	struct ServerInfo
	{
		Address address;
		int client_count;
		bool is_broadcasting;
		seconds time_to_live;
	};

	class NetworkSystem
		: public ISystem
	{
		static constexpr unsigned short server_listen_port = 8080;
		static constexpr unsigned short client_listen_port = 8000;
	public:
		NetworkSystem();
		~NetworkSystem();

		void InstantiateServer(const Address& d);
		void ConnectToServer(const Address& d);

		void Disconnect();
		void EvictClient(int clientID);

		Client& GetClient() { return *client; }
		Server& GetServer() { return *lobby; }
		array<ConnectionManager*, 5> GetConnectionManagers();

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

		// for discovery
		void SetSearch(bool enable);
		bool IsSearching() const { return static_cast<bool>(client_listen_socket); }
		void SetBroadcast(bool enable);
		bool IsBroadcasting() const { return static_cast<bool>(server_broadcast_socket); }
		vector<ServerInfo> GetDiscoveredServers() const;
	private:
		struct ResponseCallback {
			void* fn_ptr;
			function<void()> callback;
		};

		unique_ptr<Server> lobby;
		unique_ptr<Client> client;
		unique_ptr<ServerConnectionManager> server_connection_manager[GameConfiguration::MAX_CLIENTS];
		unique_ptr<ClientConnectionManager> client_connection_manager;

		unique_ptr<IDManager> id_manager;

		vector<ResponseCallback> frame_start_callbacks;
		vector<ResponseCallback> frame_end_callbacks;

		vector<Handle<mono::Behavior>> callback_objects;

		SeqNo frame_counter{};
		Host my_id = Host::NONE;

		unique_ptr<Socket> client_listen_socket;
		unique_ptr<Socket> server_broadcast_socket;

		static constexpr seconds server_broadcast_limit = seconds{ 1 };
		static constexpr seconds server_entry_time_to_live = seconds{ 5 };
		seconds server_timer{};
		std::map<Address, ServerInfo> client_address_cooldown;

		void Init() override;
		void LateInit() override;
		void EarlyShutdown() override;
		void Shutdown() override;

		void ResetNetwork();
	};
}