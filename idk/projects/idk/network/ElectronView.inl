#pragma once
#include "ElectronView.h"
#include <reflect/reflect.h>
#include <math/arith.inl>
#include <ds/result.inl>
#include <serialize/binary.inl>

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

		void UnpackMove(string_view data) override
		{
			if (auto deser = parse_binary<T>(data))
				param.setter(param.getter() + *deser);
		}

		void ApplyLerp(real delta_t) override
		{
			delta_t /= interp_over;
			t = std::min(t + delta_t, real{ 1 });
			param.setter(param.interpolator(start_value, end_value, t));
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
	};

	template<typename T>
	inline ParameterImpl<T>& ElectronView::RegisterMember(ParameterImpl<T> param, float interp)
	{
		auto ptr = std::make_unique<DerivedParameter<T>>(param);
		auto& impl = ptr->param;
		parameters.emplace_back(std::move(ptr))->interp_over = interp;
		return impl;
	}
}