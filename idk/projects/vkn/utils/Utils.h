#include "pch.h"
#pragma once
#include <vector>
#include <iosfwd>
#include <string>
#include <array>

namespace idk {
	template<typename T, uint32_t N>
	uint32_t ArrCount(T(&)[N]);
	template<typename T, uint32_t N>
	uint32_t ArrCount(std::array<T, N>const&);
	template<typename T>
	uint32_t ArrCount(std::vector<T> const& v);
	template<typename T>
	uint32_t ArrCount(T& a);


	template<typename T>
	decltype(std::declval<T>().data()) ArrData(T& a);
	template<typename T, uint32_t N>
	T* ArrData(T(&)[N]);
	template<typename T, uint32_t N>
	T* ArrData(std::array<T, N>const&);
	template<typename T>
	const T* ArrData(std::vector<T> const& v);
	template<typename T, uint32_t N>
	T* ArrData(std::array<T, N>& arr);
	template<typename T>
	T* ArrData(std::vector<T>& v);

	namespace utl
	{
		std::ostream& cerr();
	}

	std::string inline GetBinaryFile(const std::string& filepath);


}
#include "Utils.inl"