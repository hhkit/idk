#pragma once
#include <network/ack_utils.h>
#pragma optimize("", off)
namespace idk
{
	template<typename T>
	struct ElectronView::DerivedParameter<T>::DerivedControlObjectData
		: ElectronView::BaseParameter::ControlObjectData
	{
		struct SeqAndObj
		{
			SeqNo seq;
			T move;
			int move_type;
		};

		ParameterImpl<T>& param;
		circular_buffer<SeqAndObj, RememberedMoves> move_buffer;
		SeqNo latest_move;

		DerivedControlObjectData(ParameterImpl<T>& impl)
			: param{ impl }
		{
		}

		void Init(SeqNo initial_frame) override
		{
		}

		__declspec(noinline) int UnpackMove(span<const SeqAndPack> packs)
		{
			int unpacked_moves = 0;
			// assume the packs are in order
			for (auto& elem : packs)
			{
				if (move_buffer.empty() || latest_move < elem.seq)
				{
					if (auto unpacked_move = parse_binary<T>(elem.pack))
					{
						SeqAndObj new_entry;
						new_entry.seq = elem.seq;
						new_entry.move = *unpacked_move;
						new_entry.move_type = elem.move_type;
						move_buffer.emplace_back(new_entry);

						latest_move = elem.seq;

						++unpacked_moves;
					}
				}
			}
			return unpacked_moves;
		}

		void ApplyMove() override
		{
			if (move_buffer.size())
			{
				auto apply_move = move_buffer.front();
				switch (apply_move.move_type)
				{
				case SeqAndPack::set_move:
					param.setter(apply_move.move);
					break;
				case SeqAndPack::delta_move:
					param.setter(param.adder(param.getter(), apply_move.move));
					break;
				default:
					break;
				}

				move_buffer.pop_front();
			}
		}

		void VisitMoveBuffer(const BufferVisitor& visit) override
		{
			for (auto& elem : move_buffer)
				visit(elem.move, elem.seq);
		}
	};
}
#pragma optimize("", on)