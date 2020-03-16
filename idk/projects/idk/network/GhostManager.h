#pragma once
#include <network/SubstreamManager.h>
#include <network/ElectronView.h>
#include <ds/circular_buffer.inl>
namespace idk
{
	class GhostAcknowledgementMessage;

	class GhostManager
		: public SubstreamManager<GhostManager>
	{
	public:
		void SubscribeEvents(ClientConnectionManager& client) override;
		void SubscribeEvents(ServerConnectionManager& server) override;

		void SendGhosts(span<ElectronView> views);
		void SendServerCorrections(span<ElectronView> views);
	private:
		struct GhostEntry
		{
			Handle<ElectronView> id;
			unsigned state_mask{};
		};

		struct GhostPacketInfo
		{
			vector<GhostEntry> entries;
			SeqNo sequence_number;
			time_point send_time;
			bool acked = false;
		};

		circular_buffer<GhostPacketInfo, 32> transmission_record;

		void OnGhostReceived(class GhostMessage& msg);
		void OnGhostACKReceived(GhostAcknowledgementMessage& msg);
	};
}