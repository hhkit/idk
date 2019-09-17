#pragma once
#include <idk.h>
#include <file/PathHandle.h>
#include <res/ResourceFactory.h>
#include <test/TestResource.h>

namespace idk
{
	class TestResourceFactory
		: public ResourceFactory<TestResource>
	{
	public:
		unique_ptr<TestResource> GenerateDefaultResource()   override { return std::make_unique<TestResource>(); };
		unique_ptr<TestResource> Create()                    override { return GenerateDefaultResource(); };
		unique_ptr<TestResource> Create(PathHandle)          override { return std::make_unique<TestResource>(); };
	};
}