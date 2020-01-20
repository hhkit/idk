#include "stdafx.h"
#include "Server.h"
#include "TestMessage.h"

static const int MAX_PLAYERS = 4;

namespace idk
{
	Server::Server(const Address& address)
		: adapter{ this }, server{ yojimbo::GetDefaultAllocator(), DEFAULT_PRIVATE_KEY, yojimbo::Address{address.a, address.b, address.c, address.d, address.port}, config, adapter, 0.0 }
	{
		server.Start(MAX_PLAYERS);
		if (!server.IsRunning())
		{
			LOG_CRASH_TO(LogPool::NETWORK, "Could not create server!");
			throw;
		}

		char buffer[256];
		server.GetAddress().ToString(buffer, sizeof(buffer));
		LOG_TO(LogPool::NETWORK, "Server address is %s", buffer);

		// setup lobby
	}
	Server::~Server()
	{
		server.Stop();
	}
	void Server::ProcessMessage(int clientIndex, yojimbo::Message* message)
	{
		switch (message->GetType()) {
		case (int)GameMessageType::TEST:
		{
			LOG_TO(LogPool::NETWORK, "Received from %d: TestMessage with payload: %d", clientIndex, ((TestMessage*)message)->m_data);
			TestMessage* testMessage = (TestMessage*)server.CreateMessage(clientIndex, (int)GameMessageType::TEST);
			testMessage->m_data = ((TestMessage*)message)->m_data;
			server.SendMessage(clientIndex, (int)GameChannel::RELIABLE, testMessage);
			break;
		}
		default:
			break;
		}
	}

	void Server::ProcessMessages()
	{
		for (int i = 0; i < MAX_PLAYERS; i++) {
			if (server.IsClientConnected(i)) {
				for (int j = 0; j < config.numChannels; j++) {
					yojimbo::Message* message = server.ReceiveMessage(i, j);
					while (message != NULL) {
						ProcessMessage(i, message);
						server.ReleaseMessage(i, message);
						message = server.ReceiveMessage(i, j);
					}
				}
			}
		}
	}
	void Server::ReceivePackets()
	{
		server.AdvanceTime(server.GetTime() + Core::GetRealDT().count());
		server.ReceivePackets();
		ProcessMessages();
	}

	void Server::SendPackets()
	{
		server.SendPackets();
	}

	void Server::ClientConnected(int clientIndex)
	{
		LOG_TO(LogPool::NETWORK, "Client %d connected", clientIndex);
	}

	void Server::ClientDisconnected(int clientIndex)
	{
		LOG_TO(LogPool::NETWORK, "Client %d disconnected", clientIndex);
	}
}