#include "stdafx.h"
#include "ElectronView.inl"
#include <core/GameObject.inl>
#include <network/ElectronTransformView.h>
#include <network/ElectronRigidbodyView.h>
#include <network/NetworkSystem.h>
namespace idk
{
	ElectronView::ElectronView(const ElectronView& rhs)
	{
	}
	ElectronView::ElectronView(ElectronView&&) noexcept = default;
	ElectronView& ElectronView::operator=(const ElectronView& rhs)
	{
		throw;
	}
	ElectronView& ElectronView::operator=(ElectronView&&) noexcept = default;

	ElectronView::~ElectronView() = default;
	
	void ElectronView::Setup()
	{
		if (auto tfm_view = GetGameObject()->GetComponent<ElectronTransformView>())
			tfm_view->Start();
		if (auto rb_view = GetGameObject()->GetComponent<ElectronRigidbodyView>())
			rb_view->Start();

		for (auto& elem : parameters)
		{
			elem->latest_seq = Core::GetSystem<NetworkSystem>().GetSequenceNumber();
		}
	}

	void ElectronView::SetAsClientObject()
	{
		move_state = ElectronView::ClientObject{};
		for (unsigned i = 0; i < parameters.size(); ++i)
		{
			auto& param = parameters[i];
			IDK_ASSERT(param);
			param->CacheCurrValue();
		}
	}
	void ElectronView::UpdateMaster()
	{
		if (auto* master = std::get_if<Master>(&ghost_state))
		{
			state_mask = 0;
			for (unsigned i = 0; i < parameters.size(); ++i)
			{
				auto& param = parameters[i];
				IDK_ASSERT(param);
				if (param->ValueChanged())
					state_mask = 1 << i;
			}
		}
	}
	void ElectronView::UpdateGhost()
	{
		if (auto ghost_ = std::get_if<Ghost>(&ghost_state))
		{
			Ghost& ghost = *ghost_;
			auto advance = Core::GetDT().count();

			for (unsigned i = 0; i < parameters.size(); ++i)
			{
				if (state_mask & (1 << i))
					parameters[i]->ApplyLerp(advance);
			}
		}
	}

	vector<string> ElectronView::PackMoveData()
	{
		auto pack = vector<string>{};
		if (auto* master = std::get_if<ClientObject>(&move_state))
		{
			for (unsigned i = 0; i < parameters.size(); ++i)
			{
				auto& param = parameters[i];
				IDK_ASSERT(param);
				if (state_mask & (1 << i))
				{
					auto packed = param->PackMoveData();
					if (packed.size())
						pack.emplace_back(std::move(packed));
					param->CacheCurrValue();
				}
			}
			return pack;
		}
		return pack;
	}

	vector<string> ElectronView::PackGhostData()
	{
		auto pack = vector<string>{};
		if (auto* master = std::get_if<Master>(&ghost_state))
		{
			for (unsigned i = 0; i < parameters.size(); ++i)
			{
				auto& param = parameters[i];
				IDK_ASSERT(param);
				if (state_mask & (1 << i))
				{
					auto packed = param->PackGhostData();
					if (packed.size())
						pack.emplace_back(std::move(packed));
					param->CacheCurrValue();
				}
			}
			return pack;
		}
		return pack;
	}

	void ElectronView::UnpackGhostData(SeqNo sequence_number, span<string> data_pack)
	{
		if (auto* ghost = std::get_if<Ghost>(&ghost_state))
		{
			unsigned count = 0;
			for (unsigned i = 0; i < parameters.size(); ++i)
			{
				auto& param = parameters[i];
				IDK_ASSERT(param);
				if (state_mask & (1 << i))
				{
					auto& pack = data_pack[count++];
					if (seqno_greater_than(sequence_number, param->latest_seq))
					{
						param->latest_seq = sequence_number;
						param->UnpackGhost(pack);
					}
				}
			}
		}
	}

	void ElectronView::UnpackMoveData(SeqNo sequence_number, span<string> data_pack)
	{
		if (auto* master = std::get_if<ControlObject>(&move_state))
		{
			unsigned count = 0;
			for (unsigned i = 0; i < parameters.size(); ++i)
			{
				auto& param = parameters[i];
				IDK_ASSERT(param);
				if (state_mask & (1 << i))
				{
					param->UnpackMove(sequence_number, data_pack[count++]);
				}
			}
		}
	}

	void ElectronView::UpdateClient()
	{
		if (auto* master = std::get_if<ClientObject>(&move_state))
		{
			state_mask = 0;
			for (unsigned i = 0; i < parameters.size(); ++i)
			{
				auto& param = parameters[i];
				IDK_ASSERT(param);
				if (param->ValueChanged())
					state_mask = 1 << i;
			}
		}
	}
}
