#pragma once
#include <res/ResourceFactory.h>

namespace idk
{
	template<typename T>
	class EasyFactory
		: public ResourceFactory<typename T::BaseResource>
	{
	public:
		unique_ptr<typename T::BaseResource> GenerateDefaultResource() override { return std::make_unique<T>(); }
		unique_ptr<typename T::BaseResource> Create()                  override { return std::make_unique<T>(); }
	};
}