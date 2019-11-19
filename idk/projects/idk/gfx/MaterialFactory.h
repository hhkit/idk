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
		unique_ptr<Material> GenerateDefaultResource() override;
		unique_ptr<Material> Create() override;
		unique_ptr<Material> Create(PathHandle h) override;
	};

	class MaterialLoader
		: public IFileLoader
	{
	public:
		ResourceBundle LoadFile(PathHandle p, const MetaBundle& m) override;
	};
}