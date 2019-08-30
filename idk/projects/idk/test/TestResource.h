#pragma once
#include <res/ResourceMeta.h>

namespace idk
{
	struct TestMeta
	{
		int i;
		int j;
	};

	class TestResource
		: public Resource<TestResource>
		, public MetaTag<TestMeta>
	{
	public:
		int k;
		void OnMetaUpdate(const TestMeta& newmeta) override { (newmeta); k = meta.i * meta.j; }
	};
}