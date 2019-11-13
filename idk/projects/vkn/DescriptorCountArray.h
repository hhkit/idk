#pragma once
#include <array>
#include <vkn/vulkan_enum_info.h>
namespace idk::vkn
{
	using DsCountArray = std::array<uint32_t, DescriptorTypeI::size()>;

	inline DsCountArray operator*(const DsCountArray& lhs, uint32_t scalar)
	{
		auto result = lhs;
		for (auto& count : result)
		{
			count *= scalar;
		}
		return result;
	}
	inline DsCountArray operator/(const DsCountArray& lhs, uint32_t scalar)
	{
		auto result = lhs;
		for (auto& count : result)
		{
			count /= scalar;
		}
		return result;
	}
	inline DsCountArray operator*(uint32_t scalar, const DsCountArray& lhs)
	{
		auto result = lhs;
		for (auto& count : result)
		{
			count *= scalar;
		}
		return result;
	}
	inline DsCountArray operator+(const DsCountArray& lhs, const DsCountArray& rhs)
	{
		auto result = lhs;
		for (auto i=std::size(result);i-->0;)
		{
			result[i] += rhs[i];
		}
		return result;
	}
	inline DsCountArray operator-(const DsCountArray& lhs, const DsCountArray& rhs)
	{
		auto result = lhs;
		for (auto i = std::size(result); i-- > 0;)
		{
			result[i] -= rhs[i];
		}
		return result;
	}

}