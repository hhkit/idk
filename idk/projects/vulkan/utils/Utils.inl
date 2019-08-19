#pragma once
#include <utils/Utils.h>

namespace idk {
	template<typename T, uint32_t N>
	uint32_t ArrCount(T(&)[N])
	{
		return N;
	}
	template<typename T, uint32_t N>
	uint32_t ArrCount(std::array<T, N>const&)
	{
		return N;
	}
	template<typename T>
	uint32_t ArrCount(std::vector<T> const& v)
	{
		return static_cast<uint32_t>(v.size());
	}
	template<typename T>
	uint32_t ArrCount(T& a)
	{
		return static_cast<uint32_t>(a.size());
	}


	template<typename T>
	decltype(std::declval<T>().data()) ArrData(T& a)
	{
		return a.data();
	}

	template<typename T, uint32_t N>
	T* ArrData(T(&arr)[N])
	{
		return arr;
	}
	template<typename T, uint32_t N>
	const T* ArrData(std::array<T, N>const& arr)
	{
		return arr.data();
	}
	template<typename T>
	const T* ArrData(std::vector<T> const& v)
	{
		return v.data();
	}

	template<typename T, uint32_t N>
	T* ArrData(std::array<T, N>& arr)
	{
		return arr.data();
	}
	template<typename T>
	T* ArrData(std::vector<T>& v)
	{
		return v.data();
	}
}