#include "pch.h"
#include <res/ResourceManager.inl>
#include <test/TestResource.h>
#include "TestApplication.h"

TEST(Saveable, SaveableResource)
{
	using namespace idk;

    Core core;
    auto& fs = Core::GetSystem<FileSystem>();
    core.AddSystem<TestApplication>();
    core.Setup();

	auto ht = core.GetResourceManager().Create<TestResource>();

	// uncomment to test saveable
	//Core::GetSystem<SaveableResourceManager>().SaveDirtyFiles();
	//Core::GetSystem<ResourceManager>().SaveDirtyMetadata();
}