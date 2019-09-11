#pragma once

#include <idk.h>
#include <res/ExtensionLoader.h>
namespace idk::vkn
{

class VulkanGlslLoader
	: public ResourceFactory<ShaderProgram>
{
public:
	unique_ptr<ShaderProgram> GenerateDefaultResource() override;
	unique_ptr<ShaderProgram> Create(FileHandle path_to_resource) override;
	//unique_ptr<ShaderProgram> Create(FileHandle path_to_resource, const MetaFile&) override { return Create(path_to_resource); };
	//FileResources Create(FileHandle path_to_resource, const MetaFile& path_to_meta) override;
private:


};
}