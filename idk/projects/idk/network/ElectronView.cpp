#include "stdafx.h"
#include "ElectronView.inl"
#include <core/GameObject.inl>
#include <scene/SceneManager.h>
#include <scene/SceneGraph.inl>
#include <phys/RigidBody.h>

#include <network/ElectronTransformView.h>
#include <network/ElectronRigidbodyView.h>
#include <network/ElectronAnimatorView.h>
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
		move_state = ElectronView::MoveObject{};

		for (unsigned i = 0; i < parameters.size(); ++i)
		{
			auto& param = parameters[i];
			IDK_ASSERT(param);
			param->GetClientObject()->Init(Core::GetSystem<NetworkSystem>().GetSequenceNumber());
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

	void ElectronView::MoveGhost(seconds delta)
	{
		if (std::get_if<ControlObject>(&move_state))
		{
			for (unsigned i = 0; i < parameters.size(); ++i)
			{
				auto& param = parameters[i];
				param->GetControlObject()->ApplyMove();
			}
		}

		if (std::get_if<Ghost>(&ghost_state))
		{
			for (unsigned i = 0; i < parameters.size(); ++i)
			{
				auto& param = parameters[i];
				if (param->interp_over > 0.0001f)
					param->GetGhost()->Update(delta.count() / param->interp_over);
				else
					param->GetGhost()->Update(1.f);
			}
		}

		if (std::get_if<MoveObject>(&move_state))
		{
			for (unsigned i = 0; i < parameters.size(); ++i)
			{
				auto& param = parameters[i];
				param->GetClientObject()->UpdateGhost(interp_bias);
			}

		}
	}

	MovePack ElectronView::PackMoveData(SeqNo curr_seq)
	{
		MovePack move_pack;
		if (std::get_if<MoveObject>(&move_state))
		{
			move_pack.network_id = network_id;

			for (unsigned i = 0; i < parameters.size(); ++i)
			{
				auto& param = parameters[i];
				auto val = param->GetClientObject()->PackData(curr_seq);
				if (val.size())
				{
					move_pack.state_mask |= 1 << i;
					move_pack.packs.emplace_back(std::move(val));
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
				if (std::get_if<Ghost>(&ghost_state))
					param->GetGhost()->UnpackData(sequence_number, pack);
				if (std::get_if<MoveObject>(&move_state))
					param->GetClientObject()->UnpackGhost(sequence_number, pack);
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
			//	LOG_TO(LogPool::NETWORK, "unpacking %d moves", unpacked);
			}
		}
	}

	struct MoveBuffLogger
	{
		std::stringstream logger{};

		MoveBuffLogger() = default;
		MoveBuffLogger(MoveBuffLogger&&) = default;
		MoveBuffLogger& operator=(MoveBuffLogger&&) = default;

		~MoveBuffLogger()
		{
		//	LOG_TO(LogPool::NETWORK, logger.str().data());
		}

		template<typename T>
		void operator()(const T&, SeqNo) {}

		void operator()([[maybe_unused]] const vec3& v, [[maybe_unused]] SeqNo seq)
		{
		//	logger << "[" << seq.value <<"]" << (acknowledged ? "ACK" : "~~~") << "   (" << v.x <<"," << v.y <<"," << v.z <<")" << "\n";
		}

		void operator()([[maybe_unused]] const quat& v, [[maybe_unused]] SeqNo seq)
		{
		//	logger << "[" << seq.value << "]" << (acknowledged ? "ACK" : "~~~") << "   (" << v.x << "," << v.y << "," << v.z << "," << v.w << ")" << "\n";
		}
	};

	void ElectronView::DumpToLog()
	{
		auto curr_seq = Core::GetSystem<NetworkSystem>().GetSequenceNumber();
		for (auto& elem : parameters)
		{
			if (std::get_if<MoveObject>(&move_state))
				elem->GetClientObject()->VisitMoveBuffer(MoveBuffLogger{});
			if (std::get_if<ControlObject>(&move_state))
				elem->GetControlObject()->VisitMoveBuffer(MoveBuffLogger{});
		}
	}

	span<const unique_ptr<ElectronView::BaseParameter>> ElectronView::GetParameters() const
	{
		return parameters;
	}
}
