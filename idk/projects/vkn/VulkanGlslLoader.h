#pragma once

#include <idk.h>
#include <res/FileLoader.h>
namespace idk::vkn
{

	class VulkanGlslFactory
		: public ResourceFactory<ShaderProgram>
	{
	public:
		unique_ptr<ShaderProgram> GenerateDefaultResource() override;
	};

	class VulkanGlslLoader
		: public IFileLoader
	{
		ResourceBundle LoadFile(PathHandle path_to_resource) override;
		//unique_ptr<ShaderProgram> Create(PathHandle path_to_resource, const MetaFile&) override { return Create(path_to_resource); };
		//FileResources Create(PathHandle path_to_resource, const MetaFile& path_to_meta) override;
	private:


	};
}