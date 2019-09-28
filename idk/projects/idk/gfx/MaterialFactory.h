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

	class MaterialLoader
		: public IFileLoader
	{
	public:
		ResourceBundle LoadFile(PathHandle p);
		ResourceBundle LoadFile(PathHandle p, const MetaBundle& m);
	};
}