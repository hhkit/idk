#pragma once
#include "ElectronView.h"
#include <reflect/reflect.h>
#include <math/arith.inl>
#include <ds/result.inl>
#include <ds/circular_buffer.inl>
#include <serialize/binary.inl>
#include <reflect/reflect.inl>
#include <network/NetworkSystem.h>
#include <network/ConnectionManager.h>
#include <debug/Log.h>
#include <sstream>
namespace idk
{

	template<typename T>
	struct ElectronView::DerivedParameter
		: ElectronView::BaseParameter
	{
		static constexpr auto RememberedMoves = 4;

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
			T presented_value;
			T latest_value;
			T prev_value;

			DerivedGhostData(ParameterImpl<T>& p)
				: param{ p }
			{
				start_value = presented_value = latest_value = prev_value = param.getter();
			}

			void ForceUnpack(string_view data) final
			{
				if (auto val = parse_binary<T>(data))
				{
					t = 1;
					start_value = param.getter();
					latest_value = *val;
					presented_value = *val;
				}
			}

			void UnpackData(SeqNo index, string_view data) final
			{
				// newer data has arrived
				if (index > value_index)
				{
					if (auto pval = parse_binary<T>(data))
					{
						latest_value = *pval;
						// interp between vals
						auto diff = (index - value_index);
						auto diff_dt = diff * Core::GetDT();
						auto rtt = Core::GetSystem<NetworkSystem>().GetConnectionTo(Host::SERVER)->GetRTT();
						presented_value = param.interpolator(prev_value, latest_value, (rtt + diff_dt) / diff_dt);
						prev_value = latest_value;
						t = 0;
						start_value = param.getter();
						value_index = index;
					}
				}
			}

			void Update(real dt, real prediction_weight) final
			{
				// if interpolation function is defined
				if (param.interpolator)
				{
					auto new_t = std::min(t + dt, 1.f);
					if (t != new_t)
					{
						t = new_t;
						auto predicted_val = param.interpolator(start_value, presented_value, t);
						auto final_val = param.interpolator(latest_value, predicted_val, prediction_weight);
						param.setter(final_val);
					}
				}
				else // else snap to value
				{
					Snap();
				}
			}

			void Snap() final
			{
				t = 1;
				param.setter(latest_value);
			}

			virtual void Debug(erased_visitor<void(bool), void(int), void(float), void(vec3), void(quat)> visitor) final
			{
				visitor(presented_value);
			}

		};

		struct DerivedMoveObjectData;
		struct DerivedControlObjectData;

		ParameterImpl<T> param;
		DerivedMasterData master_data;
		DerivedGhostData  ghost_data;

		DerivedParameter(ParameterImpl<T> param_impl)
			: param{ std::move(param_impl) }
			, ghost_data          { param }
			, master_data         { param }
		{
			master_data.cached_value = param.getter();
		}

		DerivedMasterData* GetMaster() override { return &master_data; }
		DerivedGhostData*  GetGhost()  override { return &ghost_data; }
	};

	template<typename T>
	ElectronView::BaseParameter* ElectronView::RegisterMember(string_view name, ParameterImpl<T> param, float interp)
	{
		auto ptr = std::make_unique<DerivedParameter<T>>(param);
		ptr->param_name = string{ name };

		auto& emplaced = parameters.emplace_back(std::move(ptr)); 
		emplaced->interp_over = interp;
		return emplaced.get();
	}
}