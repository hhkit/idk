#pragma once

#include <idk.h>
#include <prefab/Prefab.h>
#include <res/ResourceFactory.h>

namespace idk
{
	class PrefabFactory : public ResourceFactory<Prefab>
	{
		unique_ptr<Prefab> Create() override;
		unique_ptr<Prefab> Create(FileHandle filepath) override;
	};
}