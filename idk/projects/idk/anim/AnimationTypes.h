#pragma once


namespace idk::anim
{
	template<typename T>
	struct AnimationParam
	{
		string name{};
		bool valid = false;

		T def_val{};
		T val{};


		void ResetToDefault() { val = def_val; }
	};

	struct IntParam : public AnimationParam<int> {};
	struct FloatParam : public AnimationParam<float> {};
	struct BoolParam : public AnimationParam<bool> {};
	struct TriggerParam : public AnimationParam<bool> {};

	inline static IntParam null_int_param{ "", false, 0, 0 };
	inline static FloatParam null_float_param{ "", false, 0, 0 };
	inline static BoolParam null_bool_param{ "", false, false, false };
	inline static TriggerParam null_trigger_param{ "", false, false, false };

	template<typename T>
	static inline T& null_param()
	{
		if constexpr (std::is_same_v<T, anim::IntParam>)
		{
			return null_int_param;
		}
		else if constexpr (std::is_same_v<T, anim::FloatParam>)
		{
			return null_float_param;
		}
		else if constexpr (std::is_same_v<T, anim::BoolParam>)
		{
			return null_bool_param;
		}
		else if constexpr (std::is_same_v<T, anim::TriggerParam>)
		{
			return null_trigger_param;
		}
		else
		{
			throw("???");
		}
	}
}