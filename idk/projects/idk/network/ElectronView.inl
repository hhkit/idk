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

		function<T()>       getter;
		function<void(const T&)> setter;
		function <T(const T&, const T&, real)> lerper;
		function<bool(const T&, const T&)> equater = std::equal_to{};

		template<typename Hnd, typename Mem>
		DerivedParameter(Handle<Hnd> obj, T(Mem::* ptr))
		{
			getter = [obj, ptr]() -> T
			{
				return std::invoke(ptr, *obj);
			};

			setter = [obj, ptr](const T& val) -> void
			{
				std::invoke(ptr, *obj) = val;
			};

			if constexpr (std::is_same_v<T, vec3>)
				lerper = &lerp<vec3, real>;
			if constexpr (std::is_same_v<T, quat>)
				lerper = static_cast<T(*)(const quat&, const quat&, real)>(slerp<quat, real>);

			cached_value = getter();
		}

		DerivedParameter(function<T()> get, function<void(const T&)> set)
			: getter {get}, setter{set}
		{
			if constexpr (std::is_same_v<T, vec3>)
				lerper = &lerp<vec3, real>;
			if constexpr (std::is_same_v<T, quat>)
				lerper = static_cast<T(*)(const quat&, const quat&, real)>(slerp<quat, real>);

			cached_value = getter();
		}

		void CacheCurrValue() override
		{
			cached_value = getter();
		}

		bool ValueChanged() const
		{
			return !equater(cached_value, getter());
		}

		void UnpackGhost(string_view data) override
		{
			if (auto deser = parse_binary<T>(data))
			{
				if (interp_over != 0)
				{
					t = 0;
					start_value = getter();
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
				setter(getter() + *deser);
		}

		void ApplyLerp(real delta_t) override
		{
			delta_t /= interp_over;
			t = std::min(t + delta_t, real{ 1 });
			auto curr_val = lerper(start_value, end_value, t);
			setter(curr_val);
		}

		string PackMoveData() override
		{
			auto del = getter() - cached_value;
			if (!equater(del, T{}))
				return serialize_binary(del);
			return {};
		}

		string PackGhostData() override
		{
			auto curr = getter();
			if (ValueChanged())
				return serialize_binary(curr);
			else
				return { };
		}

	};

	template<typename Hnd, typename Mem, typename Obj>
	inline void ElectronView::RegisterMember(Handle<Hnd> obj, Obj(Mem::* ptr), float interp_over)
	{
		parameters.emplace_back(std::make_unique<DerivedParameter<Obj>>(obj, ptr));
	}
	template<typename Val>
	inline void ElectronView::RegisterMember(function<Val()> getter, function<void(const Val&)> setter, float interp)
	{
		parameters.emplace_back(std::make_unique<DerivedParameter<Val>>(getter, setter));
	}
}