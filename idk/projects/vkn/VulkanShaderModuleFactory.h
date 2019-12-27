#pragma once
#include <idk.h>
#include <res/FileLoader.h>
#include <gfx/ShaderProgram.h>
#include <gfx/IShaderProgramFactory.h>
#include <res/ResourceFactory.h>

namespace idk::vkn
{
	class VulkanShaderModuleFactory
		: public IShaderProgramFactory
	{
	public:
		ShaderBuildResult BuildGLSL(const RscHandle<ShaderProgram>& program, ShaderStage stage, string_view glsl_code) override;
		unique_ptr<ShaderProgram> GenerateDefaultResource() override;
		unique_ptr<ShaderProgram> Create() override;
	};

	class VulkanShaderModuleLoader
		: public IFileLoader
	{
	public:
		ResourceBundle LoadFile(PathHandle filepath, const MetaBundle& bundle) override;
	};

}