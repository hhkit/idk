#include "stdafx.h"
#include "ElectronView.inl"
#include <core/GameObject.inl>
#include <network/ElectronTransformView.h>
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
	}

	void ElectronView::SetAsClientObject()
	{
		network_data = ElectronView::ClientObject{};
		for (unsigned i = 0; i < parameters.size(); ++i)
		{
			auto& param = parameters[i];
			IDK_ASSERT(param);
			param->CacheCurrValue();
		}
	}
	void ElectronView::UpdateMaster()
	{
		if (auto* master = std::get_if<Master>(&network_data))
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
		if (auto ghost_ = std::get_if<Ghost>(&network_data))
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
		if (auto* master = std::get_if<ClientObject>(&network_data))
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
		if (auto* master = std::get_if<Master>(&network_data))
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
				}
			}
			return pack;
		}
		return pack;
	}

	void ElectronView::UnpackGhostData(span<string> data_pack)
	{
		if (auto* ghost = std::get_if<Ghost>(&network_data))
		{
			unsigned count = 0;
			for (unsigned i = 0; i < parameters.size(); ++i)
			{
				auto& param = parameters[i];
				IDK_ASSERT(param);
				if (state_mask & (1 << i))
				{
					param->UnpackGhost(data_pack[count++]);
				}
			}
		}
	}

	void ElectronView::UnpackMoveData(span<string> data_pack)
	{
		if (auto* master = std::get_if<Master>(&network_data))
		{
			unsigned count = 0;
			for (unsigned i = 0; i < parameters.size(); ++i)
			{
				auto& param = parameters[i];
				IDK_ASSERT(param);
				if (state_mask & (1 << i))
				{
					param->UnpackMove(data_pack[count++]);
				}
			}
		}
	}

	void ElectronView::CacheMasterValues()
	{
		if (std::get_if<Master>(&network_data) || std::get_if<ClientObject>(&network_data))
		{
			for (unsigned i = 0; i < parameters.size(); ++i)
			{
				auto& param = parameters[i];
				IDK_ASSERT(param);
				param->CacheCurrValue();
			}
		}
	}
	void ElectronView::UpdateClient()
	{
		if (auto* master = std::get_if<ClientObject>(&network_data))
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
