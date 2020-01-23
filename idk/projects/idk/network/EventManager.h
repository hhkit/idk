#pragma once
#include <network/SubstreamManager.h>

namespace idk
{
	class EventManager
		: public SubstreamManager<EventManager>
	{
	public:
		void SubscribeEvents(ClientConnectionManager& client) override;
		void SubscribeEvents(ServerConnectionManager& server) override;

		void SendTestMessage(int i);
		void SendInstantiatePrefabEvent();
	private:
		ConnectionManager* connection_manager = nullptr;
	};
}