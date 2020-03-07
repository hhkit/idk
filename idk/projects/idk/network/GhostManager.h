#pragma once
#include <network/SubstreamManager.h>
#include <network/ElectronView.h>
namespace idk
{
	class GhostManager
		: public SubstreamManager<GhostManager>
	{
	public:
		void SubscribeEvents(ClientConnectionManager& client) override;
		void SubscribeEvents(ServerConnectionManager& server) override;

		void SendGhosts(Host target, span<ElectronView> views);
	private:
		void OnGhostReceived(class GhostMessage& msg);
	};
}