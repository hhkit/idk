#pragma once
#include <res/ResourceFactory.h>

namespace idk
{
	template<typename T>
	class EasyFactory
		: public ResourceFactory<typename T::BaseResource>
	{
	public:
		unique_ptr<typename T::BaseResource> GenerateDefaultResource() override 
		{ 
			if constexpr (std::is_default_constructible_v<T>)
			{
				auto retval = std::make_unique<T>();
				return retval;
			}
			else
				if constexpr (std::is_same_v<Scene, T>)
					return std::make_unique<T>(128);
				else
				{
					assert(false);
					return unique_ptr<T>();
				}
		}
		unique_ptr<typename T::BaseResource> Create()                  override 
		{
			if constexpr (std::is_default_constructible_v<T>)
			{
				auto retval = std::make_unique<T>();
				if constexpr (has_tag_v<T, Saveable>)
					retval->Dirty();
				return retval;
			}
			else
				if constexpr (std::is_same_v<Scene, T>)
					return std::make_unique<T>(128);
				else
				{
					assert(false);
					return unique_ptr<T>();
				}
		}
	};
}