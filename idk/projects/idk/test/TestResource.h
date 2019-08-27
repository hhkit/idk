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
		: public Resource<TestResource>,
		MetaTag<TestMeta>
	{
	public:
		int k;
		void OnMetaUpdate(const TestMeta& newmeta) override { k = meta.i * meta.j; }
	};
}