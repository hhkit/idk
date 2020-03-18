#pragma once

namespace idk
{
	template<typename T>
	struct ElectronView::DerivedParameter<T>::DerivedClientObjectData
		: ElectronView::BaseParameter::ClientObjectData
	{
		static constexpr auto move_limit = 3;
		struct SeqAndMove
			: SeqAndPack
		{
			T move;
			int send_count = 3;
			bool acknowledged = false;
		};

		ParameterImpl<T>& param;
		circular_buffer<SeqAndMove, RememberedMoves> buffer;
		T prev_value;

		DerivedClientObjectData(ParameterImpl<T>& impl)
			: param{ impl }
		{
		}

		void Init() override
		{
			prev_value = param.getter();
		}

		void CalculateMove(SeqNo curr_seq) override
		{
			auto curr_value = param.getter();

			// if the value has changed
			if (!param.equater(prev_value, curr_value))
			{
				// create the new move and cache the curr_value
				auto move = param.differ(curr_value, prev_value);
				prev_value = curr_value;

				// if the buffer is full, drop the first move
				if (buffer.capacity() == buffer.size())
				{
					if (buffer.front().acknowledged == false)
						param.setter(param.adder(param.getter(), param.differ(T{}, buffer.front().move)));

					buffer.pop_front();
				}

				// pack the move into the move buffer
				{
					SeqAndMove pushme;
					pushme.seq = curr_seq;
					pushme.pack = serialize_binary(move);
					pushme.move = move;
					buffer.emplace_back(std::move(pushme));
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
					retval.emplace_back(elem);
					--elem.send_count;
				}
			}
			return retval;
		}

		void ReceiveAcks(span<SeqNo> acknowleged_sequences) override
		{
			// newer data has arrived
			auto buffer_itr = buffer.begin();
			auto ack_itr = acknowleged_sequences.begin();

			// assume the move buffer is in ascending sequence order
			// assume the acknowledged sequences are in ascending sequence order
			while (buffer_itr != buffer.end() && ack_itr != acknowleged_sequences.end())
			{
				// if buffer is behind
				while (buffer_itr != buffer.end() && buffer_itr->seq < *ack_itr)
					buffer_itr++;

				// if ack is behind
				while (ack_itr != acknowleged_sequences.end() && buffer_itr->seq > *ack_itr)
					ack_itr++;

				// if we reached the end
				if (buffer_itr == buffer.end() || ack_itr == acknowleged_sequences.end())
					break;

				if (buffer_itr->seq == *ack_itr)
					buffer_itr->acknowledged = true;

				ack_itr++;
			}
		}

		void UnpackGhost(SeqNo index, string_view data) override
		{
			if (index > last_received)
			{
				if (auto val = parse_binary<T>(data))
				{
					auto retain_move = param.differ(param.getter(), prev_value);

					T accumulated_moves{};
					for (auto& elem : buffer)
					{
						if (elem.acknowledged == false)
							accumulated_moves = param.adder(accumulated_moves, elem.move);
					}

					auto corrected_current_value = *val + accumulated_moves + retain_move;

					// correct current value
					param.setter(corrected_current_value);

					// shift the prev value to account for the corrected value
					prev_value = param.differ(corrected_current_value, retain_move);

					// cleanup
					last_received = index;
				}
			}
		}
		void VisitMoveBuffer(const BufferVisitor& visit) override
		{
			for (auto& elem : buffer)
				visit(elem.move, elem.seq, elem.acknowledged);
		}
	};
}