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
			bool verified = false;
		};

		ParameterImpl<T>& param;
		T prev_value;
		circular_buffer<SeqAndObj, RememberedMoves> move_buffer;

		DerivedControlObjectData(ParameterImpl<T>& impl)
			: param{ impl }
		{
			prev_value = param.getter();
		}

		void Init() override
		{
			prev_value = param.getter();
		}

		unsigned AcknowledgeMoves(SeqNo curr_seq) override
		{
			vector<SeqNo> ack_us;

			for (auto& elem : move_buffer)
			{
				if (elem.verified)
					ack_us.emplace_back(elem.seq);
			}

			return acks_to_ackfield(curr_seq, ack_us);
		}

		__declspec(noinline) void RecordPrediction(SeqNo curr_seq) override
		{
			auto curr_value = param.getter();
			auto new_move = param.differ(curr_value, prev_value);
			prev_value = curr_value;

			move_buffer.emplace_back(SeqAndObj{ curr_seq, new_move });
		}

		void ApplyCorrection(typename circular_buffer<SeqAndObj, RememberedMoves>::iterator itr, const T& real_move)
		{
			switch (param.predict_func)
			{
			case PredictionFunction::Quadratic:
			case PredictionFunction::Linear:
			{
				auto old_val = param.getter();
				auto move_change = param.differ(real_move, itr->move);

				prev_value = param.adder(prev_value, move_change);
				param.setter(param.adder(param.getter(), move_change));
				break;
			}
			};

		}

		__declspec(noinline) int UnpackMove(span<const SeqAndPack> packs)
		{
			int unpacked_moves = 0;
			for (auto& elem : packs)
			{
				if (auto unpacked_move = parse_binary<T>(elem.pack))
				{
					auto& real_move = *unpacked_move;

					// compare with move
					// if necessary, displace existing data
					for (auto itr = move_buffer.begin(); itr != move_buffer.end(); ++itr)
					{
						auto& prediction = *itr;
						if (prediction.verified)
							continue;

						if (prediction.seq == elem.seq)
						{
							// if prediction was wrong
							if (!param.equater(prediction.move, real_move))
								ApplyCorrection(itr, real_move);

							itr->verified = true;
							++unpacked_moves;
							break;
						}
					}
				}
			}
			return unpacked_moves;
		}

		void VisitMoveBuffer(const BufferVisitor& visit) override
		{
			for (auto& elem : move_buffer)
				visit(elem.move, elem.seq, elem.verified);
		}
	};
}
#pragma optimize("", on)