#pragma once

#include <idk.h>
#include <res/FileLoader.h>

namespace idk::vkn
{
	class ShaderModule;

class VulkanSpvLoader
	: public IFileLoader
{
public:

	//FileResources Create(PathHandle path_to_resource) override;
	//FileResources Create(PathHandle path_to_resource, const MetaFile& path_to_meta) override { }

	ResourceBundle LoadFile(PathHandle path, RscHandle<ShaderModule> module) ;
	ResourceBundle LoadFile(PathHandle path_to_resource) override;
	ResourceBundle LoadFile(PathHandle handle, const MetaBundle& meta) override;
	//unique_ptr<ShaderProgram> Create(PathHandle path_to_resource, const MetaFile&) override { return Create(path_to_resource); };
	//FileResources Create(PathHandle path_to_resource, const MetaFile& path_to_meta) override;
private:


};

	class VulkanGlslLoader
		: public IFileLoader
	{
		ResourceBundle LoadFile(PathHandle path_to_resource) override;
		ResourceBundle LoadFile(PathHandle handle, const MetaBundle& meta) override;
		//unique_ptr<ShaderProgram> Create(PathHandle path_to_resource, const MetaFile&) override { return Create(path_to_resource); };
		//FileResources Create(PathHandle path_to_resource, const MetaFile& path_to_meta) override;
	};
}