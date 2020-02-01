#pragma once
#include <yojimbo/yojimbo.h>

namespace idk
{
	class Server;

	class Adapter
		: public yojimbo::Adapter
	{
	public:
		Adapter(Server* server = nullptr) : server{ server } {}
        yojimbo::MessageFactory* CreateMessageFactory(yojimbo::Allocator& allocator) override;
		void OnServerClientConnected(int clientIndex) override;
		void OnServerClientDisconnected(int clientIndex) override;

	private:
		Server* server{};
	};
}