#pragma once
#include <network/GameConfiguration.h>
#include <network/SubstreamManager.h>

namespace idk
{
	class ClientMoveManager
		: public SubstreamManager<MoveManager>
	{
	public:
		void SubscribeEvents(ClientConnectionManager& client) override;
		void SubscribeEvents(ServerConnectionManager& server) override;

	private:
		static void CachePreviousPositions(span<ElectronView> views);
		void SendMoves(span<ElectronView> views);
	};
}