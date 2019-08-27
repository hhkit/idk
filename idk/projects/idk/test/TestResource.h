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
		void OnMetaUpdate() override { k = meta.i * meta.j; }
	};
}