#pragma once
#include <yojimbo/yojimbo.h>
#include <core/ISystem.h>

namespace idk
{
	class Client;
	class Server;

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
		bool IsHost();

		Client& GetClient() { return *client; }
		Server& GetServer() { return *lobby; }

		void ReceivePackets();
		void SendPackets();
	private:
		std::unique_ptr<Server> lobby;
		std::unique_ptr<Client> client;

		void Init() override;
		void LateInit() override;
		void Shutdown() override;

		void ResetNetwork();
	};
}