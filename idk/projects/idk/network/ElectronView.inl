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
		T cached_value;
		T start_value;
		T end_value;

		ParameterImpl<T> param;

		DerivedParameter(ParameterImpl<T> param_impl)
			: param{param_impl}
		{
		}

		void CacheCurrValue() override
		{
			cached_value = param.getter();
		}

		bool ValueChanged() const
		{
			return param.send_condition(cached_value, param.getter());
		}

		void UnpackGhost(string_view data) override
		{
			// if we already have the latest information, ignore the incoming info
			if (auto deser = parse_binary<T>(data))
			{
				if (interp_over != 0)
				{
					t = 0;
					start_value = param.getter();
					end_value = *deser;
				}
				else
				{
					t = 1;
					end_value = *deser;
				}
			}
		}

		void UnpackMove(SeqNo seq_number, string_view data) override
		{
			if (auto deser = parse_binary<T>(data))
				param.setter(param.getter() + *deser);
		}

		void ApplyLerp(real delta_t) override
		{
			if (interp_over != 0 && param.interpolator)
			{
				delta_t /= interp_over;
				t = std::min(t + delta_t, real{ 1 });
				param.setter(param.interpolator(start_value, end_value, t));
			}
			else
				param.setter(end_value);
		}

		string PackMoveData() override
		{
			auto del = param.getter() - cached_value;
			if (!param.equater(del, T{}))
				return serialize_binary(del);
			return {};
		}

		string PackGhostData() override
		{
			auto curr = param.getter();
			if (ValueChanged())
				return serialize_binary(curr);
			else
				return { };
		}

		reflect::dynamic GetParam() override
		{
			return param.getter();
		}
	};

	template<typename T>
	inline ParameterImpl<T>& ElectronView::RegisterMember(string_view name, ParameterImpl<T> param, float interp)
	{
		auto ptr = std::make_unique<DerivedParameter<T>>(param);
		ptr->param_name = string{ name };
		auto& impl = ptr->param;
		parameters.emplace_back(std::move(ptr))->interp_over = interp;
		return impl;
	}
}