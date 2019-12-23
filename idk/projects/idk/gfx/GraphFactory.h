#pragma once
#include <idk.h>
#include <res/ResourceFactory.h>
#include <gfx/ShaderGraph.h>
#include <res/FileLoader.h>
namespace idk
{
	class GraphFactory
		: public ResourceFactory<Material>
	{
	public:
		unique_ptr<Material> GenerateDefaultResource() override;
		unique_ptr<Material> Create() override;
	};

	class GraphLoader
		: public IFileLoader
	{
	public:
		ResourceBundle LoadFile(PathHandle p);
		ResourceBundle LoadFile(PathHandle p, const MetaBundle& m);
	};
}