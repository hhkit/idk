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

		void SendGhosts(span<ElectronTransformView> views); // by right should be just ElectronView but we deal with that later
	private:
		void OnGhostReceived(class GhostMessage* msg);
	};
}