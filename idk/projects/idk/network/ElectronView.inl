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

		struct DerivedMoveObjectData;
		struct DerivedControlObjectData;

		ParameterImpl<T> param;
		DerivedMasterData master_data;
		DerivedGhostData  ghost_data;
		DerivedMoveObjectData client_object_data;
		DerivedControlObjectData control_object_data;

		DerivedParameter(ParameterImpl<T> param_impl)
			: param{ std::move(param_impl) }
			, ghost_data          { param }
			, master_data         { param }
			, client_object_data  { param }
			, control_object_data { param }
		{
			master_data.cached_value = param.getter();
		}

		DerivedControlObjectData* GetControlObject() override { return &control_object_data; }
		DerivedMoveObjectData* GetClientObject() override { return &client_object_data; };
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

#include <network/ElectronView_DerivedMoveObject.h>
#include <network/ElectronView_DerivedControlObject.h>