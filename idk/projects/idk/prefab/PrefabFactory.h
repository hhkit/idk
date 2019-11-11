#pragma once

#include <idk.h>
#include <prefab/Prefab.h>
#include <res/EasyFactory.h>
#include <res/ResourceFactory.h>

namespace idk
{
	using PrefabFactory = EasyFactory<Prefab>;
}