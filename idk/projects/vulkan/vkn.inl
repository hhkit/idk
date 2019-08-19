#pragma once
#include <vulkan/vkn.h>

namespace idk {
	template<typename T, typename F>
	bool Vulkan::CheckProperties(std::vector<T> const& properties, std::vector<const char*> const& desired, const F& func)
	{
		bool result = true;
		std::unordered_set<std::string> mprop;
		for (auto& prop : properties)
		{
			mprop.emplace(func(prop));
		}
		for (auto& desire : desired)
		{
			if (mprop.find(desire) == mprop.end())
			{
				result = false;
				break;
			}
		}
		return result;
	}
}