#pragma once
#include <idk.h>
#include <gfx/ShaderProgram.h>
#include <res/ResourceFactory.h>

namespace idk::vkn
{
	class VulkanShaderModuleFactory
		: public ResourceFactory<ShaderProgram>
	{
	public:
		unique_ptr<ShaderProgram> GenerateDefaultResource() override;
		unique_ptr<ShaderProgram> Create(PathHandle filepath) override;
	private:
	};

}