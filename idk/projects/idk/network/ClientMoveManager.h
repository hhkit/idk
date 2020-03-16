#pragma once
#include <network/GameConfiguration.h>
#include <network/SubstreamManager.h>
#include <network/MoveAcknowledgementMessage.h>

namespace idk
{
	class GhostMessage;

	class ClientMoveManager
		: public SubstreamManager<MoveManager>
	{
	public:
		void SubscribeEvents(ClientConnectionManager& client) override;
		void SubscribeEvents(ServerConnectionManager& server) override;
		void SendMoves(span<ElectronView> views);
	private:
		void OnGhostReceived(GhostMessage& ghost_msg);
		void OnMoveAcknowledgementReceived(MoveAcknowledgementMessage& move_msg);
	};
}