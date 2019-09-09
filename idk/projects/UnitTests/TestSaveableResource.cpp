#include "pch.h"
#include <test/TestResource.h>
#include <res/SaveableResourceManager.h>

TEST(Saveable, SaveableResource)
{
	using namespace idk;

	Core c;
	c.Setup();

	auto ht = c.GetResourceManager().Create<TestResource>();

	// uncomment to test saveable
	//Core::GetSystem<SaveableResourceManager>().SaveDirtyFiles();
	//Core::GetSystem<ResourceManager>().SaveDirtyMetadata();
}