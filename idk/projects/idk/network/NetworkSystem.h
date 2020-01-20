#pragma once
#include <yojimbo/yojimbo.h>
#include <core/ISystem.h>

namespace idk
{

	class NetworkSystem
		: public ISystem
	{
		static constexpr unsigned short server_listen_port = 8000;
		static constexpr unsigned short client_listen_port = 8080;
	public:
		NetworkSystem();
		~NetworkSystem();

		void InstantiateServer(const Address& d);
		void ConnectToServer(const Address& d);
		bool IsHost();

		class Client& GetClient() { return *client; }
		class Server& GetServer() { return *lobby; }

		void ReceivePackets();
		void SendPackets();
	private:
		std::unique_ptr<class Server> lobby;
		std::unique_ptr<class Client> client;

		void Init() override;
		void LateInit() override;
		void Shutdown() override;

		void ResetNetwork();
	};
}