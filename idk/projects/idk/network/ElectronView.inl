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
					auto move = param.differ(curr_value, prev_value);
					prev_value = curr_value;
					SeqAndMove pushme;
					pushme.seq = curr_seq;
					pushme.pack = serialize_binary(move);
					pushme.move = move;
					buffer.emplace_back(std::move(pushme));
				}

				// discard ancient moves
				auto discard_age = static_cast<SeqNo>(curr_seq < move_limit ? std::numeric_limits<SeqNo>::max() - (move_limit - 1 - curr_seq) : curr_seq - 3);
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
			struct SeqAndObj { SeqNo seq; T obj; bool verified = false; };

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
					auto change = param.differ(real_move, itr->obj);
					param.setter(param.adder(param.getter(), change));
					prev_value = param.adder(prev_value, change);
					// and snap
					itr->obj = real_move;
					break;
				}
				case PredictionFunction::Quadratic:
				if (itr != move_buffer.end())
				{
					auto diff = param.differ(real_move, itr->obj);

					while (itr != move_buffer.end())
					{
						if (itr->verified)
							break;

						itr->obj = param.adder(itr->obj, diff);
						param.setter(param.adder(param.getter(), diff));
						prev_value = param.adder(prev_value, diff);
						++itr;
					}
					break;
				}
				};

			}

			__declspec(noinline) void UnpackMove(span<const SeqAndPack> packs)
			{
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
							if (prediction.seq == elem.seq)
							{
								// if prediction was wrong
								if (!param.equater(prediction.obj, real_move))
								{
									// calculate correction
									ApplyCorrection(itr, real_move);
									itr->verified = true;
								}
								break;
							}
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