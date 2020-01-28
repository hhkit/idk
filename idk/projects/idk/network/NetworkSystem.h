#pragma once
#include <yojimbo/yojimbo.h>
#include <core/ISystem.h>
#include <network/Address.h>
#include <network/GameConfiguration.h>
#include <event/Signal.h>

namespace idk
{
	class Client;
	class Server;
	class ConnectionManager;
	class IDManager;
	class ClientConnectionManager;
	class ServerConnectionManager;

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

		Client& GetClient() { return *client; }
		Server& GetServer() { return *lobby; }

		bool IsHost();
		ConnectionManager& GetConnectionManager(size_t token = GameConfiguration::MAX_CLIENTS);
		IDManager& GetIDManager() { return *id_manager; }

		void ReceivePackets();
		void SendPackets();

		void RespondToPackets();
		void PreparePackets();
	private:
		unique_ptr<Server> lobby;
		unique_ptr<Client> client;
		unique_ptr<ServerConnectionManager> server_connection_manager[GameConfiguration::MAX_CLIENTS];
		unique_ptr<ClientConnectionManager> client_connection_manager;

		unique_ptr<IDManager> id_manager;

		void Init() override;
		void LateInit() override;
		void Shutdown() override;

		void ResetNetwork();
	};
}