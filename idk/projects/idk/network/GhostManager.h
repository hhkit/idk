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

		static void UpdateGhosts(span<ElectronView>);
		static void UpdateMasters(span<ElectronView>);

		void SendGhosts(span<ElectronView> views); // by right should be just ElectronView but we deal with that later
	private:
		void OnGhostReceived(class GhostMessage* msg);
	};
}