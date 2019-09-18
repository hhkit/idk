#pragma once

#include <idk.h>
#include <prefab/Prefab.h>
#include <res/ResourceFactory.h>

namespace idk
{
	class PrefabFactory : public ResourceFactory<Prefab>
	{
    public:
        unique_ptr<Prefab> GenerateDefaultResource() override;
		unique_ptr<Prefab> Create() override;
		unique_ptr<Prefab> Create(PathHandle filepath) override;
	};
}