#pragma once
namespace idk
{

	template<typename Ptr, Ptr ptr>
	struct PtrMem;

	//Imposes a strict ordering on the members
	template<typename T, typename...PtrToMems>
	struct ordered_comparator
	{
		bool operator()(const T& lhs, const T& rhs)const noexcept;
	};

}
