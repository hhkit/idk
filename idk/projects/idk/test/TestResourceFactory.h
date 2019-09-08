#pragma once
#include <idk.h>
#include <file/FileHandle.h>
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
		unique_ptr<TestResource> Create(FileHandle)          override { return std::make_unique<TestResource>(); };
	};
}