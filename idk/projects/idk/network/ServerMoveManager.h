#pragma once
#include <network/GameConfiguration.h>
#include <network/SubstreamManager.h>

namespace idk
{
	class ServerMoveManager
		: public SubstreamManager<MoveManager>
	{
	public:
		void SubscribeEvents(ClientConnectionManager& client) override;
		void SubscribeEvents(ServerConnectionManager& server) override;

	private:
		void OnMoveReceived(MoveClientMessage&);
	};
}