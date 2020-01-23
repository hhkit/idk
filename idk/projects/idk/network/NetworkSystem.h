#pragma once
#include <yojimbo/yojimbo.h>
#include <core/ISystem.h>
#include <network/Address.h>
#include <event/Signal.h>

namespace idk
{
	class Client;
	class Server;
	class ClientConnectionManager;
	class ServerConnectionManager;

	class NetworkSystem
		: public ISystem
	{
		static constexpr unsigned short server_listen_port = 8080;
		static constexpr unsigned short client_listen_port = 8000;
	public:
		Signal<> OnServerCreate;
		Signal<> OnClientCreate;

		NetworkSystem();
		~NetworkSystem();

		void InstantiateServer(const Address& d);
		void ConnectToServer(const Address& d);
		bool IsHost();

		Client& GetClient() { return *client; }
		Server& GetServer() { return *lobby; }
		ClientConnectionManager& GetClientConnectionManager() { return *client_connection_manager; }
		ServerConnectionManager& GetServerConnectionManager() { return *server_connection_manager; }

		void ReceivePackets();
		void SendPackets();
	private:
		std::unique_ptr<Server> lobby;
		std::unique_ptr<Client> client;
		std::unique_ptr<ServerConnectionManager> server_connection_manager;
		std::unique_ptr<ClientConnectionManager> client_connection_manager;

		void Init() override;
		void LateInit() override;
		void Shutdown() override;

		void ResetNetwork();
	};
}