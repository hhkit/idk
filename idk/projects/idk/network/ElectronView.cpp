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
			param->GetClientObject()->last_received = Core::GetSystem<NetworkSystem>().GetSequenceNumber();
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
	void ElectronView::PrepareDataForSending(SeqNo curr_seq)
	{
		state_mask = 0;

		if (std::get_if<ClientObject>(&move_state))
		{
			for (unsigned i = 0; i < parameters.size(); ++i)
			{
				auto& param = parameters[i];
				param->GetClientObject()->CalculateMove(curr_seq);
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

	void ElectronView::MoveGhost(seconds delta)
	{
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
	}

	MovePack ElectronView::PackMoveData(SeqNo curr_seq)
	{
		MovePack move_pack;
		if (std::get_if<ClientObject>(&move_state))
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

	ControlGhost ElectronView::PackServerGuess(int incoming_state_mask, SeqNo curr_seq) const
	{
		const auto transmit_state_mask = incoming_state_mask | state_mask;
		ControlGhost retval;
		retval.network_id = network_id;
		retval.sequence_number = curr_seq;

		for (unsigned i = 0; i < parameters.size(); ++i)
		{
			const auto sm = transmit_state_mask & (1 << i);
			if (sm)
			{
				auto& param = parameters[i];
				if (std::get_if<ControlObject>(&move_state))
				{
					retval.ackfield |= param->GetControlObject()->AcknowledgeMoves(curr_seq);
					retval.verified_ghost_value.emplace_back(param->GetControlObject()->GetGhostValue());
					retval.state_mask |= sm;
				}
			}
		}

		return retval;
	}

	void ElectronView::UnpackServerGuess(const ControlGhost& control_ghost)
	{
		auto acks = ackfield_to_acks(control_ghost.sequence_number, control_ghost.ackfield);

		auto value_index = 0;
		for (unsigned i = 0; i < parameters.size(); ++i)
		{
			if (control_ghost.state_mask & (1 << i))
			{
				auto& param = parameters[i];
				auto& val = control_ghost.verified_ghost_value[value_index++];
				if (std::get_if<ClientObject>(&move_state))
				{
					param->GetClientObject()->ReceiveAcks(acks);
					param->GetClientObject()->UnpackGhost(control_ghost.sequence_number, val);
				}
			}
		}

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
				auto unpacked = parameters[i]->GetControlObject()->UnpackMove(move_pack);
				LOG_TO(LogPool::NETWORK, "unpacking %d moves", unpacked);
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
			LOG_TO(LogPool::NETWORK, logger.str().data());
		}

		template<typename T>
		void operator()(const T&, SeqNo, bool) {}

		void operator()(const vec3& v, SeqNo seq, bool acknowledged)
		{
		//	logger << "[" << seq.value <<"]" << (acknowledged ? "ACK" : "~~~") << "   (" << v.x <<"," << v.y <<"," << v.z <<")" << "\n";
		}

		void operator()(const quat& v, SeqNo seq, bool acknowledged)
		{
		//	logger << "[" << seq.value << "]" << (acknowledged ? "ACK" : "~~~") << "   (" << v.x << "," << v.y << "," << v.z << "," << v.w << ")" << "\n";
		}
	};

	void ElectronView::DumpToLog()
	{
		erased_visitor<void(int)> hey{ [](int a) {} };

		auto curr_seq = Core::GetSystem<NetworkSystem>().GetSequenceNumber();
		for (auto& elem : parameters)
		{
			if (std::get_if<ClientObject>(&move_state))
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
