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

		void ReceivePackets();
		void SendPackets();
	private:
		std::unique_ptr<Server> lobby;
		std::unique_ptr<Client> client;
		std::unique_ptr<ServerConnectionManager> server_connection_manager[GameConfiguration::MAX_CLIENTS];
		std::unique_ptr<ClientConnectionManager> client_connection_manager;

		void Init() override;
		void LateInit() override;
		void Shutdown() override;

		void ResetNetwork();
	};
}