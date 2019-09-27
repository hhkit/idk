#pragma once
#include <idk.h>
#include <gfx/ShaderProgram.h>
#include <res/ResourceFactory.h>

namespace idk::vkn
{
	class VulkanShaderModuleLoader
		: public IFileLoader
	{
	public:
		ResourceBundle LoadFile(PathHandle filepath) override;
		ResourceBundle LoadFile(PathHandle filepath, const MetaBundle& bundle) override;
	};

}