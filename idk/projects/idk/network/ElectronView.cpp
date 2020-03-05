#include "stdafx.h"
#include "ElectronView.inl"
#include <core/GameObject.inl>

#include <phys/RigidBody.h>

#include <network/ElectronTransformView.h>
#include <network/ElectronRigidbodyView.h>
#include <network/NetworkSystem.h>
namespace idk
{
	ElectronView::ElectronView(const ElectronView&)
	{
	}
	ElectronView::ElectronView(ElectronView&&) noexcept = default;
	ElectronView& ElectronView::operator=(const ElectronView&)
	{
		throw;
	}
	ElectronView& ElectronView::operator=(ElectronView&&) noexcept = default;

	ElectronView::~ElectronView() = default;
	
	bool ElectronView::IsMine() const
	{
		return Core::GetSystem<NetworkSystem>().GetMe() == owner;
	}

	void ElectronView::Setup()
	{
		if (auto tfm_view = GetGameObject()->GetComponent<ElectronTransformView>())
			tfm_view->Start();
		if (auto rb_view = GetGameObject()->GetComponent<ElectronRigidbodyView>())
			rb_view->Start();

		for (auto& elem : parameters)
		{
			elem->GetGhost()->value_index = Core::GetSystem<NetworkSystem>().GetSequenceNumber();
			elem->GetMaster()->last_packed = Core::GetSystem<NetworkSystem>().GetSequenceNumber();
		}
	}

	void ElectronView::SetAsClientObject()
	{
		ghost_state = std::monostate{};
		move_state = ElectronView::ClientObject{};
		if (auto e_rb = GetGameObject()->GetComponent<ElectronRigidbodyView>())
		{
			if (e_rb->sync_velocity)
				if (const auto rb = GetGameObject()->GetComponent<RigidBody>())
					rb->is_kinematic = false;
		}
		for (unsigned i = 0; i < parameters.size(); ++i)
		{
			auto& param = parameters[i];
			IDK_ASSERT(param);
		}
	}
	void ElectronView::UpdateStateFlags()
	{
		if (auto* master = std::get_if<Master>(&ghost_state))
		{
			state_mask = 0;
			for (unsigned i = 0; i < parameters.size(); ++i)
			{
				auto& param = parameters[i];
				IDK_ASSERT(param);
				if (param->GetMaster()->ValueChanged())
					state_mask = 1 << i;
			}
		}
	}
	void ElectronView::UpdateGhost()
	{
		if (auto ghost_ = std::get_if<Ghost>(&ghost_state))
		{
			auto advance = Core::GetDT().count();

			for (unsigned i = 0; i < parameters.size(); ++i)
			{
				if (state_mask & (1 << i))
					parameters[i]->GetGhost()->Update(advance);
			}
		}
	}

	vector<string> ElectronView::PackMoveData()
	{
		return {};
	}

	GhostPack ElectronView::MasterPackData(int incoming_state_mask)
	{
		auto transmit_state_mask = incoming_state_mask | state_mask;
		IDK_ASSERT(std::get_if<Master>(&ghost_state));
		auto seq = Core::GetSystem<NetworkSystem>().GetSequenceNumber();
		GhostPack retval;
		retval.network_id = network_id;
		retval.state_mask = transmit_state_mask;
		retval.data_packs.reserve(parameters.size());
		for (unsigned i = 0; i < parameters.size(); ++i)
		{
			auto& param = parameters[i];
			IDK_ASSERT(param);
			if (transmit_state_mask & (1 << i))
			{
				auto packed = param->GetMaster()->PackData(seq);
				if (packed.size())
					retval.data_packs.emplace_back(std::move(packed));
			}
		}
		return retval;
	}

	void ElectronView::UnpackGhostData(SeqNo sequence_number, const GhostPack& ghost_pack)
	{
		state_mask = ghost_pack.state_mask;
		unsigned pack_index{};

		for (unsigned i = 0; i < parameters.size(); ++i)
		{
			if (state_mask & (1 << i))
			{
				auto& pack = ghost_pack.data_packs[pack_index++];
				parameters[i]->GetGhost()->UnpackData(sequence_number, pack);
			}
		}
	}

	void ElectronView::UnpackMoveData(SeqNo sequence_number, span<string> data_pack)
	{
	}

	hash_table<string, reflect::dynamic> ElectronView::GetParameters() const
	{
		auto retval = hash_table<string, reflect::dynamic>();
		retval.reserve(parameters.size());
		return retval;
	}

	void ElectronView::UpdateClient()
	{
		if (auto* master = std::get_if<ClientObject>(&move_state))
		{
		}
	}
}
