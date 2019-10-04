#pragma once
#include <res/ResourceMeta.h>
#include <res/SaveableResource.h>

namespace idk
{
	struct TestMeta
	{
		int i;
		int j;
	};

RESOURCE_EXTENSION(TestResource, ".test")

	class TestResource
		: public Resource<TestResource>
		, public MetaTag<TestMeta>
		, public Saveable<TestResource>
	{
	public:
        int k = 0;
		string yolo = "haha";
		void OnMetaUpdate(const TestMeta& newmeta) override { (newmeta); k = meta.i * meta.j; }
	};
}