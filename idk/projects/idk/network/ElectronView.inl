#pragma once
#include "ElectronView.h"
#include <reflect/reflect.h>
#include <math/arith.inl>
#include <ds/result.inl>
#include <ds/circular_buffer.inl>
#include <serialize/binary.inl>
#include <reflect/reflect.inl>
#include <debug/Log.h>
#include <sstream>
namespace idk
{

	template<typename T>
	struct ElectronView::DerivedParameter
		: ElectronView::BaseParameter
	{
		static constexpr auto RememberedMoves = 8;

		struct DerivedMasterData
			: MasterData
		{
			ParameterImpl<T>& param;
			T cached_value;
			DerivedMasterData(ParameterImpl<T>& impl)
				: param{ impl } 
			{
				cached_value = param.getter();
			}

			bool ValueChanged() const override
			{
				return !param.equater(param.getter(), cached_value);
			}

			string PackData() override
			{
				return serialize_binary(param.getter());
			}

			void CacheValue(SeqNo pack_date) override
			{
				last_packed = pack_date;
				cached_value = param.getter();
			}
		};

		struct DerivedGhostData
			: GhostData
		{
			ParameterImpl<T>& param;
			T start_value;
			T end_value;

			DerivedGhostData(ParameterImpl<T>& p)
				: param{ p }
			{
				start_value = param.getter();
				end_value = param.getter();
			}

			void UnpackData(SeqNo index, string_view data) override
			{
				// newer data has arrived
				if (seqno_greater_than(index, value_index))
				{
					if (auto val = parse_binary<T>(data))
					{
						value_index = index;
						t = 0;
						start_value = param.getter();
						end_value = *val;
					}
				}
			}

			void Update(real dt) override
			{
				// if interpolation function is defined
				if (param.interpolator)
				{
					auto new_t = std::min(t + dt, 1.f);
					if (t != new_t)
					{
						t = new_t;
						param.setter(param.interpolator(start_value, end_value, t));
					}
				}
				else // else snap to value
				{
					t = 1;
					param.setter(end_value);
				}
			}

		};

		struct DerivedClientObjectData
			: ClientObjectData
		{
			static constexpr auto move_limit = 3;
			struct SeqAndMove
				: SeqAndPack
			{
				T move;
				int send_count    = 3;
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
				if (!param.equater(prev_value, curr_value))
				{
					auto move = param.differ(curr_value, prev_value);
					prev_value = curr_value;

					// if the buffer is full, drop the first move
					if (buffer.capacity() == buffer.size())
					{
						if (buffer.front().acknowledged == false)
							param.setter(param.adder(param.getter(), param.differ(T{}, buffer.front().move)));

						buffer.pop_front();
					}

					// pack the move
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
					// if butter is behind
					while (buffer_itr != buffer.end() && buffer_itr->seq < *ack_itr)
						buffer_itr++;

					while (ack_itr != acknowleged_sequences.end() && buffer_itr->seq > * ack_itr)
						ack_itr++;

					if (buffer_itr == buffer.end() || ack_itr == acknowleged_sequences.end())
						break;

					if (buffer_itr->seq == *ack_itr)
						buffer_itr->acknowledged = true;

					if (buffer_itr->seq < *ack_itr)
						buffer_itr++;
					else
						ack_itr++;
				}
			}

			void UnpackGhost(SeqNo index, string_view data) override
			{
				if (index > last_received)
				{
					if (auto val = parse_binary<T>(data))
					{
						T move{};
						for (auto& elem : buffer)
						{
							if (elem.acknowledged == false)
								move = param.adder(move, elem.move);
						}
						auto curr_val = *val + move;
						prev_value = curr_val;
						param.setter(curr_val);
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
		struct DerivedControlObjectData
			: ControlObjectData
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

			int AcknowledgeMoves(SeqNo curr_seq) override
			{
				int mask{};

				for (auto& elem : move_buffer)
				{
					if (elem.verified)
					{
						auto seq = elem.seq;
						auto diff = curr_seq - seq;
						if (diff < 32)
							mask |= 1 << diff;
					}
				}

				return mask;
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
				case PredictionFunction::Linear:
				{
					auto old_val = param.getter();
					auto change = param.differ(real_move, itr->move);
					param.setter(param.adder(param.getter(), change));
					prev_value = param.adder(prev_value, change);
					auto new_val = param.getter();

					if constexpr (std::is_same_v<vec3, T>)
						LOG_TO(LogPool::NETWORK, "MOVE: (%f, %f, %f)    OLD: (%f, %f, %f)    NEW: (%f, %f, %f)    CHANGE: (%f, %f, %f)",
							real_move.x, real_move.y, real_move.z,
							old_val.x, old_val.y, old_val.z,
							new_val.x, new_val.y, new_val.z,
							change.x, change.y, change.z);
					// and snap
					itr->move = real_move;
					break;
				}
				case PredictionFunction::Quadratic:
				if (itr != move_buffer.end())
				{
					auto diff = param.differ(real_move, itr->move);

					while (itr != move_buffer.end())
					{
						if (itr->verified)
							break;

						itr->move = param.adder(itr->move, diff);
						param.setter(param.adder(param.getter(), diff));
						prev_value = param.adder(prev_value, diff);
						++itr;
					}
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

		ParameterImpl<T> param;
		DerivedMasterData master_data;
		DerivedGhostData  ghost_data;
		DerivedClientObjectData client_object_data;
		DerivedControlObjectData control_object_data;

		DerivedParameter(ParameterImpl<T> param_impl)
			: param{ param_impl }
			, ghost_data          { param }
			, master_data         { param }
			, client_object_data  { param }
			, control_object_data { param }
		{
			master_data.cached_value = param.getter();
		}

		DerivedControlObjectData* GetControlObject() override { return &control_object_data; }
		DerivedClientObjectData* GetClientObject() override { return &client_object_data; };
		DerivedMasterData* GetMaster() override { return &master_data; }
		DerivedGhostData*  GetGhost()  override { return &ghost_data; }
	};

	template<typename T>
	ParameterImpl<T>& ElectronView::RegisterMember(string_view name, ParameterImpl<T> param, float interp)
	{
		auto ptr = std::make_unique<DerivedParameter<T>>(param);
		ptr->param_name = string{ name };
		auto& impl = ptr->param;
		parameters.emplace_back(std::move(ptr))->interp_over = interp;
		return impl;
	}
}