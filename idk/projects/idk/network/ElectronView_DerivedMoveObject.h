#pragma once

namespace idk
{
	template<typename T>
	struct ElectronView::DerivedParameter<T>::DerivedMoveObjectData
		: ElectronView::BaseParameter::MoveObjectData
	{
		static constexpr auto move_limit = 3;
		struct SeqAndMove
			: SeqAndPack
		{
			T move;
		};

		ParameterImpl<T>& param;
		circular_buffer<SeqAndMove, RememberedMoves> buffer;
		T ghost_value;

		DerivedMoveObjectData(ParameterImpl<T>& impl)
			: param{ impl }
		{
			ghost_value = param.getter();
		}

		void Init(SeqNo initial_frame) override
		{
			ghost_value = param.getter();
			last_received = initial_frame;
		}

		virtual void PushMove(SeqNo curr_seq, int move_type, const SyncableValue& value) override
		{
			auto val = std::get<T>(value); // we let c++ variant throw here in case we make any runtime errors

			SeqAndMove new_move;
			new_move.seq = curr_seq;
			new_move.move_type = move_type;
			new_move.move = val;

			if (buffer.empty() || buffer.back().seq < curr_seq)
				buffer.emplace_back(new_move);
			else
			{
				switch (move_type)
				{
				case SeqAndPack::set_move:
					// if the previous move this frame was a delta move, we change it to a set move
					param.setter(val);
					buffer.back() = new_move;
					break;

				case SeqAndPack::delta_move: 
				{
					auto& back = buffer.back();
					back.move = param.adder(back.move, val);
					param.setter(back.move);
					// if the previous move was a set move, we compound the delta but remain a set move
					break;
				}
				default:
					break;
				}
			}
		}

		small_vector<SeqAndPack> PackData(SeqNo curr_seq) override
		{
			small_vector<SeqAndPack> retval;
			for (auto& elem : buffer)
			{
				if (elem.send_count > 0)
				{
					// if unserialized, serialize now
					if (elem.pack.empty())
						elem.pack = serialize_binary(elem.move);

					retval.emplace_back(elem);
					--elem.send_count;
				}
			}

			while (buffer.size() && buffer.front().send_count == 0)
				buffer.pop_front();

			return retval;
		}

		void UnpackGhost(SeqNo index, string_view data) override
		{
			if (index > last_received)
			{
				if (auto valid_data = parse_binary<T>(data))
				{
					ghost_value = *valid_data;
					last_received = index;
				}
			}
		}

		void Update(real ghost_bias) override
		{
			param.setter(param.interpolator(param.getter(), ghost_value, ghost_bias));
		}

		void VisitMoveBuffer(const BufferVisitor& visit) 
		{
			for (auto& elem : buffer)
				visit(elem.move, elem.seq);
		}
	};
}