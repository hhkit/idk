#pragma once
#include <idk.h>
#include <file/PathHandle.h>
#include <res/EasyFactory.h>
#include <test/TestResource.h>

namespace idk
{
	using TestResourceFactory = EasyFactory<TestResource>;
}