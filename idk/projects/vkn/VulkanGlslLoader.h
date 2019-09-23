#pragma once

#include <idk.h>
#include <res/ExtensionLoader.h>
namespace idk::vkn
{
	
class VulkanSpvLoader
	: public ExtensionLoader
{
public:

	FileResources Create(PathHandle path_to_resource) override;
	FileResources Create(PathHandle path_to_resource, const MetaFile& path_to_meta) override { return Create(path_to_resource); }

	//unique_ptr<ShaderProgram> Create(PathHandle path_to_resource, const MetaFile&) override { return Create(path_to_resource); };
	//FileResources Create(PathHandle path_to_resource, const MetaFile& path_to_meta) override;
private:


};
class VulkanGlslLoader
	: public ExtensionLoader
{
public:
	FileResources Create(PathHandle path_to_resource) override;
	FileResources Create(PathHandle path_to_resource, const MetaFile& path_to_meta) override { return Create(path_to_resource); }
	
	//unique_ptr<ShaderProgram> Create(PathHandle path_to_resource, const MetaFile&) override { return Create(path_to_resource); };
	//FileResources Create(PathHandle path_to_resource, const MetaFile& path_to_meta) override;
private:


};
}