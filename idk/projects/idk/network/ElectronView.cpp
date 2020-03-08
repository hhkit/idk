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

		for (unsigned i = 0; i < parameters.size(); ++i)
		{
			auto& param = parameters[i];
			IDK_ASSERT(param);
			param->GetClientObject()->Init();
		}
	}
	void ElectronView::CacheSentData()
	{
		if (std::get_if<Master>(&ghost_state))
		{

			const auto curr_seq = Core::GetSystem<NetworkSystem>().GetSequenceNumber();
			for (auto& elem : parameters)
				elem->GetMaster()->CacheValue(curr_seq);
		}
	}
	void ElectronView::PrepareDataForSending()
	{
		state_mask = 0;
		const auto curr_seq = Core::GetSystem<NetworkSystem>().GetSequenceNumber();

		if (std::get_if<ClientObject>(&move_state))
		{
			for (unsigned i = 0; i < parameters.size(); ++i)
			{
				auto& param = parameters[i];
				param->GetClientObject()->CalculateMoves(curr_seq);
			}
		}

		if (std::get_if<ControlObject>(&move_state))
		{
			for (unsigned i = 0; i < parameters.size(); ++i)
			{
				auto& param = parameters[i];
				param->GetControlObject()->RecordPrediction(curr_seq);
			}
		}

		if (std::get_if<Master>(&ghost_state))
		{
			for (unsigned i = 0; i < parameters.size(); ++i)
			{
				auto& param = parameters[i];
				IDK_ASSERT(param);
				if (auto master = param->GetMaster(); master && master->ValueChanged())
					state_mask |= 1 << i;
			}
		}
	}

	void ElectronView::MoveGhost()
	{
		if (std::get_if<Ghost>(&ghost_state))
		{
			auto advance = Core::GetDT().count();

			for (unsigned i = 0; i < parameters.size(); ++i)
				parameters[i]->GetGhost()->Update(advance);
		}
	}

	MovePack ElectronView::PackMoveData()
	{
		MovePack move_pack;
		if (std::get_if<ClientObject>(&move_state))
		{
			move_pack.network_id = network_id;

			for (unsigned i = 0; i < parameters.size(); ++i)
			{
				auto& param = parameters[i];
				auto val = param->GetClientObject()->PackData();
				if (val.size())
				{
					move_pack.state_mask |= 1 << i;
					move_pack.packs.emplace_back(std::move(val));
				}
			}
		}
		return move_pack;
	}

	GhostPack ElectronView::MasterPackData(int incoming_state_mask)
	{
		const auto transmit_state_mask = incoming_state_mask | state_mask;
		IDK_ASSERT(std::get_if<Master>(&ghost_state));
		const auto seq = Core::GetSystem<NetworkSystem>().GetSequenceNumber();
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
				auto packed = param->GetMaster()->PackData();
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

	void ElectronView::UnpackMoveData(const MovePack& data_pack)
	{
		const auto sm = data_pack.state_mask;
		auto pack_ptr = 0;

		for (unsigned i = 0; i < parameters.size(); ++i)
		{
			if (sm & (1 << i))
			{
				auto& move_pack = data_pack.packs[pack_ptr++];
				parameters[i]->GetControlObject()->UnpackMove(move_pack);
			}
		}
	}

	hash_table<string, reflect::dynamic> ElectronView::GetParameters() const
	{
		auto retval = hash_table<string, reflect::dynamic>();
		retval.reserve(parameters.size());
		return retval;
	}
}
