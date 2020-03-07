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
			{}

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
			};

			ParameterImpl<T>& param;
			circular_buffer<SeqAndMove, move_limit> buffer;
			T prev_value;

			DerivedClientObjectData(ParameterImpl<T>& impl)
				: param{ impl }
			{}

			void Init() override
			{
				prev_value = param.getter();
			}

			void CalculateMoves(SeqNo curr_seq) override
			{
				auto curr_value = param.getter();
				if (!param.equater(prev_value, curr_value))
				{
					auto move = curr_value - prev_value;
					prev_value = curr_value;
					SeqAndMove pushme;
					pushme.seq = curr_seq;
					pushme.pack = serialize_binary(move);
					pushme.move = move;
					buffer.emplace_back(std::move(pushme));
					if constexpr (std::is_same_v<T, vec3>)
						LOG_TO(LogPool::NETWORK, "Detected move at seq %d: (%f,%f,%f)", curr_seq, move.x, move.y, move.z);
				}

				// discard ancient moves
				auto discard_age = curr_seq < move_limit ? std::numeric_limits<SeqNo>::max() - (move_limit - 1 - curr_seq) : curr_seq - 3;
				while (buffer.size() && seqno_greater_than(discard_age, buffer.front().seq))
					buffer.pop_front();
			}

			small_vector<SeqAndPack> PackData() override
			{
				small_vector<SeqAndPack> retval;
				for (auto& elem : buffer)
					retval.emplace_back(elem);
				if (retval.size() > move_limit)
					throw;
				return retval;
			}
		};
		struct DerivedControlObjectData
			: ControlObjectData
		{
			static constexpr auto RememberedMoves = 64;
			struct SeqAndObj { SeqNo seq; T obj; };

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

			__declspec(noinline) void RecordPrediction(SeqNo curr_seq) override
			{
				auto curr_value = param.getter();
				auto new_move = curr_value - prev_value;
				prev_value = curr_value;

				if constexpr (std::is_same_v<T, vec3>)
					LOG_TO(LogPool::NETWORK, "Recording predicted value at seq %d: (%f,%f,%f)", curr_seq, new_move.x, new_move.y, new_move.z);

				move_buffer.emplace_back(SeqAndObj{ curr_seq, new_move });
			}

			__declspec(noinline) void UnpackMove(span<const SeqAndPack> packs)
			{
				for (auto& elem : packs)
				{
					if (auto unpacked_move = parse_binary<T>(elem.pack))
					{
						auto& real_move = *unpacked_move;
						bool set = false;
						// compare with move
						// if necessary, displace existing data
						for (auto& prediction : move_buffer)
						{
							if (prediction.seq == elem.seq)
							{
								// if prediction was wrong
								if constexpr (std::is_same_v<T, vec3>)
									LOG_TO(LogPool::NETWORK, "Move %d in bounds, old value (%f, %f, %f), new value (%f, %f, %f)"
										, elem.seq
										, prediction.obj.x, prediction.obj.y, prediction.obj.z
										, real_move.x, real_move.y, real_move.z);

								if (!param.equater(prediction.obj, real_move))
								{
									LOG_TO(LogPool::NETWORK, "not equal, updating predict");
									// calculate correction
									auto change = prediction.obj - real_move;
									param.setter(param.getter() - change);
									prev_value -= change;
									// and snap
									prediction.obj = real_move;
									set = true;
								}
								break;
							}
						}
						if (set)
						{
							std::stringstream s;
							s << "Move %d out of bounds [";
							for (auto& val : move_buffer)
								s << val.seq << ", ";
							s << "]";
							LOG_TO(LogPool::NETWORK, s.str().data(), elem.seq);
						}
					}
				}
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