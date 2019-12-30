#include "pch.h"
#include <res/ResourceManager.inl>
#include <test/TestResource.h>

TEST(Saveable, SaveableResource)
{
	using namespace idk;

    INIT_CORE();

	auto ht = core.GetResourceManager().Create<TestResource>();

	// uncomment to test saveable
	//Core::GetSystem<SaveableResourceManager>().SaveDirtyFiles();
	//Core::GetSystem<ResourceManager>().SaveDirtyMetadata();
}