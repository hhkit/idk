#include "stdafx.h"
#include "GhostManager.h"
#include <core/GameObject.inl>
#include <common/Transform.h>
#include <res/ResourceHandle.inl>
#include <network/GhostMessage.h>
#include <network/GhostAcknowledgementMessage.h>
#include <network/NetworkSystem.inl>
#include <network/IDManager.h>
#include <network/ElectronTransformView.h>
#include <network/ConnectionManager.inl>
#include <network/SubstreamManager.inl>
#include <network/MoveAcknowledgementMessage.h>

namespace idk
{
	void GhostManager::SubscribeEvents(ClientConnectionManager& client)
	{
		client.Subscribe<GhostMessage>([this](GhostMessage& msg) { OnGhostReceived(msg); });
	}

	void GhostManager::SubscribeEvents(ServerConnectionManager& server)
	{
		server.Subscribe<GhostAcknowledgementMessage>([this](GhostAcknowledgementMessage& msg) { OnGhostACKReceived(msg); });
	}

	void GhostManager::SendGhosts(span<ElectronView> views)
	{
		const auto now = Clock::now();
		const auto rtt = connection_manager->GetRTT();
		const auto timeout = rtt;
		const auto target_host = connection_manager->GetConnectedHost();
		const auto seq = Core::GetSystem<NetworkSystem>().GetSequenceNumber();
		// timeout packets
		hash_table<Handle<ElectronView>, int> view_to_state_masks;

		auto& id_man = Core::GetSystem<NetworkSystem>().GetIDManager();
		
		while (transmission_record.size())
		{
			if (transmission_record.front().acked)
			{
				transmission_record.pop_front();
				continue;
			}

			if (now - transmission_record.front().send_time < timeout) // not timed out
				break;

			// now we must have timed out

			for (auto& elem : transmission_record.front().entries)
			{
				auto view = elem.id;
				if (view)
				{
					const auto itr = view_to_state_masks.find(view);
					if (itr != view_to_state_masks.end())
						itr->second |= elem.state_mask;
					else
						view_to_state_masks[view] = elem.state_mask;
				}
			}

			transmission_record.pop_front();
		}

		// now we have the list of dropped, check to see if the mask was retransmitted recently
		for (auto& record : transmission_record)
		{
			for (auto& entry : record.entries)
			{
				auto find = view_to_state_masks.find(entry.id);
				if (find != view_to_state_masks.end())
					find->second &= ~entry.state_mask;
			}
		}

		vector<GhostWithMoveAck> ghost_moves_and_acks;
		vector<GhostPack> ghost_packs;

		for (auto& view : views)
		{
			if (view.network_id == 0)
				continue;

			const bool is_ghost = std::get_if<ElectronView::Master>(&view.ghost_state);
			const bool is_move_creator = std::get_if<ElectronView::ClientObject>(&view.move_state);

			if (is_ghost || is_move_creator)
			{
				auto retransmit_state_mask = 0;
				if (auto itr = view_to_state_masks.find(view.GetHandle()); itr != view_to_state_masks.end())
					retransmit_state_mask = itr->second;

				auto pack = view.MasterPackData(retransmit_state_mask);
				if (is_move_creator)
				{
					GhostWithMoveAck obj{ std::move(pack) };
					obj.ack = view.PrepareMoveAcknowledgementsAndGuess(seq);

					if (obj.ack.ackfield) // if we have naything to acknowledge
						ghost_moves_and_acks.emplace_back(std::move(obj)); // acknowledge
				}
				else
				{
					if (is_ghost)
					{
						if (pack.data_packs.size())
							ghost_packs.emplace_back(std::move(pack));
					}
				}
			}
		}

		if (ghost_packs.size())
		{
			{
				GhostPacketInfo transmission_rec;
				transmission_rec.sequence_number = seq;
				transmission_rec.send_time = now;
				for (auto& elem : ghost_packs)
					transmission_rec.entries.emplace_back(GhostEntry{ id_man.GetViewFromId(elem.network_id) , elem.state_mask});

				transmission_record.emplace_back(std::move(transmission_rec));
			}

			connection_manager->CreateAndSendMessage<GhostMessage>(GameChannel::UNRELIABLE, [&](GhostMessage& msg)
				{
					msg.sequence_number = Core::GetSystem<NetworkSystem>().GetSequenceNumber();
					msg.ghost_packs = std::move(ghost_packs);
				});
		}

		if (ghost_moves_and_acks.size())
		{
			{
				GhostPacketInfo transmission_rec;
				transmission_rec.sequence_number = seq;
				transmission_rec.send_time = now;
				for (auto& elem : ghost_moves_and_acks)
					transmission_rec.entries.emplace_back(GhostEntry{ id_man.GetViewFromId(elem.network_id) , elem.state_mask });

				transmission_record.emplace_back(std::move(transmission_rec));
			}

			connection_manager->CreateAndSendMessage<MoveAcknowledgementMessage>
				(GameChannel::UNRELIABLE, [&](MoveAcknowledgementMessage& msg)
					{
						msg.objects = std::move(ghost_moves_and_acks);
					});
		}
	}


	void GhostManager::OnGhostReceived(GhostMessage& msg)
	{
		auto seq = msg.sequence_number;

		for (auto& pack : msg.ghost_packs)
		{
			const auto& id_man = Core::GetSystem<NetworkSystem>().GetIDManager();
			
			if (const auto view = id_man.GetViewFromId(pack.network_id))
			{
				// push the ghost data into the view
				if (!std::get_if<ElectronView::Ghost>(&view->ghost_state))
					continue;

				if (std::get_if<ElectronView::ClientObject>(&view->move_state))
					continue;

				view->UnpackGhostData(seq, pack);
			}
		}

		 connection_manager->CreateAndSendMessage<GhostAcknowledgementMessage>(GameChannel::UNRELIABLE, [&](GhostAcknowledgementMessage& msg)
		 	{
		 		msg.base_ack = seq;
		 		msg.ack_field = 0; // fill later
		 	});
	}

	void GhostManager::OnGhostACKReceived(GhostAcknowledgementMessage& msg)
	{
		auto ack = msg.base_ack;
		auto ackfield = msg.ack_field;

		hash_set<SeqNo> acked{ack};

		while (ackfield)
		{
			--ack;
			
			if (ackfield & 1)
				acked.emplace(ack);
		}

		for (auto& elem : transmission_record)
		{
			if (acked.find(elem.sequence_number) != acked.end())
				elem.acked = true;
		}
	}

}