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

	ResourceBundle LoadFile(PathHandle path, RscHandle<ShaderModule> module) ;
	ResourceBundle LoadFile(PathHandle handle, const MetaBundle& meta) override;
private:


};

	class VulkanGlslLoader
		: public IFileLoader
	{
		ResourceBundle LoadFile(PathHandle handle, const MetaBundle& meta) override;
	};
}