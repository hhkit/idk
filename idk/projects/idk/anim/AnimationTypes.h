#pragma once


namespace idk::anim
{
	template<typename T>
	struct AnimationParam
	{
		string name;
		T def_val;
		T val;

		void ResetToDefault() { val = def_val; }
	};
}