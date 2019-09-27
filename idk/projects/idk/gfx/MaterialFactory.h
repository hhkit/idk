#pragma once
#include <idk.h>
#include <gfx/Material.h>
#include <res/ResourceFactory.h>

namespace idk
{
	class MaterialFactory
		: public ResourceFactory<Material>
	{
	public:
		unique_ptr<Material> GenerateDefaultResource();
		unique_ptr<Material> Create();
	};
}