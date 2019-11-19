#pragma once
#include <idk.h>
#include <res/ResourceFactory.h>
#include <gfx/ShaderGraph.h>
namespace idk
{
	class GraphFactory
		: public ResourceFactory<Material>
	{
	public:
		unique_ptr<Material> GenerateDefaultResource() override;
		unique_ptr<Material> Create() override;
		unique_ptr<Material> Create(PathHandle p) override;
	};
}