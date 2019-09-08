#include "pch.h"
#include <test/TestResource.h>
#include <res/SaveableResourceManager.h>

TEST(Saveable, SaveableResource)
{
	using namespace idk;

	Core c;
	c.Setup();

	//auto ht = c.GetResourceManager().Create<TestResource>();

	Core::GetSystem<SaveableResourceManager>().SaveDirtyFiles();
}