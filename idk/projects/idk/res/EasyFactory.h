#pragma once
#include <res/ResourceFactory.h>
#include <util/ioutils.h>

namespace idk
{
	template<typename T>
	class EasyFactory
		: public ResourceFactory<typename T::BaseResource>
	{
	public:
		using typename ResourceFactory<typename T::BaseResource>::Resource;

		unique_ptr<Resource> GenerateDefaultResource() override 
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
					IDK_ASSERT(false);
					return unique_ptr<T>();
				}
		}
		unique_ptr<Resource> Create() override 
		{
			if constexpr (std::is_default_constructible_v<T>)
			{
				auto retval = std::make_unique<T>();
				//if constexpr (has_tag_v<T, Saveable>)
				//	retval->Dirty();
				return retval;
			}
			else if constexpr (std::is_same_v<Scene, T>)
				return std::make_unique<T>(128);
			else
			{
				IDK_ASSERT(false);
				return unique_ptr<T>();
			}
		}

		unique_ptr<Resource> Create(PathHandle path_to_single_file) override
		{
			if constexpr (has_tag_v<T, Saveable>)
			{
				auto stream = path_to_single_file.Open(FS_PERMISSIONS::READ);
				auto parsed = parse_text<T>(stringify(stream));
				return parsed ? std::make_unique<T>(*parsed) : unique_ptr<T>{};
			}
			else if constexpr (std::is_same_v<Scene, T>)
				return std::make_unique<T>(128);
			else
			{
				IDK_ASSERT(false);
				return unique_ptr<T>();
			}
		}
	};
}