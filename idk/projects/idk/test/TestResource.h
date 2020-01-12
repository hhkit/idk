#pragma once
#include <res/ResourceMeta.h>
#include <res/SaveableResource.h>
#include <res/ResourceExtension.h>
namespace idk
{
	struct TestMeta
	{
		int i;
		int j;
	};

	class TestResource
		: public Resource<TestResource>
		, public Saveable<TestResource>
	{
	public:
        int k = 0;
		string yolo = "haha";
		EXTENSION(".test")
	};
}