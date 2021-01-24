#include "stdafx.h"
#include "ElectronView.inl"
#include <core/GameObject.inl>
#include <scene/SceneManager.h>
#include <scene/SceneGraph.inl>
#include <phys/RigidBody.h>
#include <script/ManagedType.h>
#include <script/MonoBehavior.h>

#include <network/ElectronTransformView.h>
#include <network/ElectronRigidbodyView.h>
#include <network/ElectronAnimatorView.h>
#include <network/NetworkSystem.h>
namespace idk
{
	ElectronView::ElectronView(const ElectronView&)
	{
	}

	ElectronView::ElectronView(ElectronView&&) = default;

	ElectronView& ElectronView::operator=(const ElectronView&)
	{
		throw;
	}

	ElectronView& ElectronView::operator=(ElectronView&&) = default;

	ElectronView::~ElectronView() = default;
	
	bool ElectronView::IsMine() const
	{
		return Core::GetSystem<NetworkSystem>().GetMe() == owner;
	}

	void ElectronView::Setup()
	{
		if (const auto tfm_view = GetGameObject()->GetComponent<ElectronTransformView>())
			tfm_view->Start();
		if (const auto rb_view = GetGameObject()->GetComponent<ElectronRigidbodyView>())
			rb_view->Start();

		if (const auto sg_handle = Core::GetSystem<SceneManager>().FetchSceneGraphFor(GetGameObject()))
		{
			sg_handle.Visit([&](Handle<GameObject> go, int) -> bool
				{
					if (auto animator_view = go->GetComponent<ElectronAnimatorView>())
					{
						animator_view->Start(GetHandle());
						return false;
					}
					for (auto& behavior : go->GetComponents<mono::Behavior>())
					{
						auto& obj = behavior->GetObject();
						auto& type = *obj.Type();
						auto method = type.GetMethod("ProcessInput", 1);
						if (auto thunk = std::get_if<mono::ManagedThunk>(&method))
						{
							observed_components.push_back(behavior);
							return false;
						}
					}
					return true;
				}
			);
		}

		for (auto& elem : parameters)
		{
			elem->GetGhost()->value_index = Core::GetSystem<NetworkSystem>().GetSequenceNumber();
			elem->GetMaster()->last_packed = Core::GetSystem<NetworkSystem>().GetSequenceNumber();
		}
	}

	void ElectronView::SetAsClientObject()
	{
		ghost_state = std::monostate{};
		move_state = ElectronView::ClientSideInputs{};
		if (const auto sg_handle = Core::GetSystem<SceneManager>().FetchSceneGraphFor(GetGameObject()))
		{
			sg_handle.Visit([&](Handle<GameObject> go, int) -> bool
			{
				if (auto animator_view = go->GetComponent<ElectronAnimatorView>())
				{
					animator_view->Start(GetHandle());
					return false;
				}
				for (auto& behavior : go->GetComponents<mono::Behavior>())
				{
					auto& obj = behavior->GetObject();
					auto& type = *obj.Type();
					auto method = type.GetMethod("ProcessInput", 1);
					if (auto thunk = std::get_if<mono::ManagedThunk>(&method))
					{
						observed_components.push_back(behavior);
						return false;
					}
				}
				return true;
			}
			);
		}
		std::sort(observed_components.begin(), observed_components.end());
		observed_components.erase(std::unique(observed_components.begin(), observed_components.end()), observed_components.end());
	}

	void ElectronView::SetAsControlObject()
	{
		ghost_state = std::monostate{};
		move_state = ServerSideInputs{};
		if (const auto sg_handle = Core::GetSystem<SceneManager>().FetchSceneGraphFor(GetGameObject()))
		{
			sg_handle.Visit([&](Handle<GameObject> go, int) -> bool
			{
				if (auto animator_view = go->GetComponent<ElectronAnimatorView>())
				{
					animator_view->Start(GetHandle());
					return false;
				}
				for (auto& behavior : go->GetComponents<mono::Behavior>())
				{
					auto& obj = behavior->GetObject();
					auto& type = *obj.Type();
					auto method = type.GetMethod("ProcessInput", 1);
					if (auto thunk = std::get_if<mono::ManagedThunk>(&method))
					{
						observed_components.push_back(behavior);
						return false;
					}
				}
				return true;
			}
			);
		}
		std::sort(observed_components.begin(), observed_components.end());
		observed_components.erase(std::unique(observed_components.begin(), observed_components.end()), observed_components.end());
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
	void ElectronView::PrepareDataForSending([[maybe_unused]] SeqNo curr_seq)
	{
		state_mask = 0;
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

	void ElectronView::MoveGhost(seconds delta, real prediction_weight)
	{
		if (std::get_if<Ghost>(&ghost_state))
		{
			for (unsigned i = 0; i < parameters.size(); ++i)
			{
				auto& param = parameters[i];
				if (param->interp_over > 0.0001f)
					param->GetGhost()->Update(delta.count() / param->interp_over, prediction_weight);
				else
					param->GetGhost()->Update(1.f, 0.0f);
			}
		}
	}

	MovePack ElectronView::PackMoveData(SeqNo curr_seq)
	{
		MovePack move_pack;
		if (auto* move_obj = std::get_if<ClientSideInputs>(&move_state))
		{
			move_pack.network_id = network_id;

			for (auto& elem : move_obj->moves)
			{
				if (elem.send_count > 0)
				{
					move_pack.packs.emplace_back(SeqAndMove{ elem.index, elem.payload });
					elem.send_count--;
				}
			}
		}

		return move_pack;
	}

	GhostPack ElectronView::PackGhostData(int incoming_state_mask)
	{
		const auto transmit_state_mask = incoming_state_mask | state_mask;
		IDK_ASSERT(std::get_if<Master>(&ghost_state));

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
				auto& param = parameters[i];
				auto& pack = ghost_pack.data_packs[pack_index++];
				param->GetGhost()->UnpackData(sequence_number, pack);
			}
		}
	}

	void ElectronView::UnpackMoveData(const MovePack& data_pack)
	{
		if (auto inputs = std::get_if<ServerSideInputs>(&move_state))
		{
			for (auto& move : data_pack.packs)
				inputs->moves.emplace(move.seq, move.move);

			while (inputs->moves.size() > 15)
				inputs->moves.pop_front();
		}
	}

	span<const unique_ptr<ElectronView::BaseParameter>> ElectronView::GetParameters() const
	{
		return parameters;
	}
}
