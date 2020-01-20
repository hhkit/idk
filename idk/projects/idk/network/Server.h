#pragma once
#include <yojimbo/yojimbo.h>
#include "Adapter.h"
#include "GameConfiguration.h"
#include "Address.h"
namespace idk
{
	class EventManager;
	class EventInstantiatePrefabPayload; //TODO: resolve this

	class Server
	{
	public:
		Server(const Address& address);
		~Server();

		void ProcessMessages();

		void ReceivePackets();
		void SendPackets();

		// managers
		EventManager& GetEventManager() { return *event_manager; }
		void SendEvent(const EventInstantiatePrefabPayload&);

		// callbacks
		void ClientConnected(int clientIndex);
		void ClientDisconnected(int clientIndex);
	private:
		Adapter           adapter;
		GameConfiguration config;
		yojimbo::Server   server;

		std::unique_ptr<EventManager> event_manager;

		void ProcessMessage(int clientIndex, yojimbo::Message* message);
	};
}