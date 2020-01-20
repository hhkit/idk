#include "Client.h"
#include <core/Core.h>
#include "TestMessage.h"
namespace idk
{
	Client::Client(const Address& server_addr)
		: client(yojimbo::GetDefaultAllocator(), yojimbo::Address("0.0.0.0"), config, adapter, 0.0)
	{
		uint64_t clientId;
		yojimbo::random_bytes((uint8_t*)&clientId, 8);
		client.InsecureConnect(DEFAULT_PRIVATE_KEY, clientId, yojimbo::Address{ server_addr.a, server_addr.b, server_addr.c, server_addr.d, server_addr.port });
	}

	Client::~Client()
	{
		if (client.IsConnected())
			client.Disconnect();
	}

	void Client::ProcessMessages()
	{
		for (int i = 0; i < config.numChannels; i++) {
			while (auto message = client.ReceiveMessage(i)) 
			{
				ProcessMessage(message);
				client.ReleaseMessage(message);
				message = client.ReceiveMessage(i);
			}
		}
	}

	void Client::ReceivePackets()
	{
		client.AdvanceTime(client.GetTime() + Core::GetRealDT().count());
		client.ReceivePackets();
		ProcessMessages();
	}
	void Client::SendPackets()
	{
		client.SendPackets();
	}

	void Client::SendTestMessage(int i )
	{
		TestMessage* testMessage = (TestMessage*)client.CreateMessage((int)GameMessageType::TEST);
		testMessage->m_data = i;
		client.SendMessage((int)GameChannel::RELIABLE, testMessage);
	}

	void Client::ProcessMessage(yojimbo::Message* message)
	{
		switch (message->GetType()) {
		case (int)GameMessageType::TEST:
			LOG_TO(LogPool::NETWORK, "Received from network: TestMessage with payload: %d", ((TestMessage*)message)->m_data);;
			break;
		default:
			break;
		}
	}
}