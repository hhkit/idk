#pragma once


namespace idk::anim
{
	template<typename T>
	struct AnimationParam
	{
		explicit AnimationParam(string_view n = "", bool v = false, const T& d_val = T{ 0 }, const T& c_val = T{ 0 })
			:name{ n }, valid{ v }, def_val{ d_val }, val{ c_val }
		{
		}

		string name{};
		bool valid = false;

		T def_val{0};
		T val{0};


		virtual void ResetToDefault() { val = def_val; }
	};

	struct IntParam : public AnimationParam<int> {
		using base = AnimationParam<int>;
		using base::base;
	};
	struct FloatParam : public AnimationParam<float> {
		using base = AnimationParam<float>;
		using base::base;
	};
	struct BoolParam : public AnimationParam<bool> {
		using base = AnimationParam<bool>;
		using base::base;
	};
	struct TriggerParam : public AnimationParam<bool> {
		using base = AnimationParam<bool>;
		using base::base;
		virtual void ResetToDefault() override { val = def_val = false; }
	};

	inline static IntParam null_int_param{  };
	inline static FloatParam null_float_param{  };
	inline static BoolParam null_bool_param{  };
	inline static TriggerParam null_trigger_param{  };

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