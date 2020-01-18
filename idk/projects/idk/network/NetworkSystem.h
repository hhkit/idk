#pragma once
#include <yojimbo/yojimbo.h>
#include <core/ISystem.h>

namespace idk
{

	class NetworkSystem
		: public ISystem
	{
	public:
		NetworkSystem();
		~NetworkSystem();
		void InstantiateServer();
		bool IsHost();

		void ReceivePackets();
		void SendPackets();
	private:
		std::unique_ptr<class Server> lobby;
		//std::unique_ptr<class Client> client;

		void Init() override;
		void Shutdown() override;
	};
}