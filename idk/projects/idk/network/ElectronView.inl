#pragma once
#include "ElectronView.h"
#include <reflect/reflect.h>
#include <math/arith.inl>
#include <ds/result.inl>
#include <serialize/binary.inl>
#include <reflect/reflect.inl>

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

			string PackData(SeqNo pack_date) override
			{
				last_packed = pack_date;
				cached_value = param.getter();
				return serialize_binary(cached_value);
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

		ParameterImpl<T> param;
		DerivedMasterData master_data;
		DerivedGhostData  ghost_data;

		DerivedParameter(ParameterImpl<T> param_impl)
			: param{ param_impl }, ghost_data{ param }, master_data{param}
		{
			master_data.cached_value = param.getter();
		}

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