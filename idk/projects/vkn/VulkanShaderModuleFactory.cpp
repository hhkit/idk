#include "pch.h"
#include <sstream>
#include "VulkanShaderModuleFactory.h"
#include <vkn/ShaderModule.h>

namespace idk::vkn
{

	unique_ptr<ShaderProgram> VulkanShaderModuleFactory::GenerateDefaultResource()
	{
		return std::make_unique<ShaderModule>();
	}
	unique_ptr<ShaderProgram> VulkanShaderModuleFactory::Create(FileHandle filepath)
	{
		auto program = std::make_unique<ShaderModule>();
		auto shader_stream = filepath.Open(FS_PERMISSIONS::READ, true);
		std::stringstream stringify;
		stringify << shader_stream.rdbuf();
		
		auto shader_enum = [](std::string_view ext)->vk::ShaderStageFlagBits
		{
			switch (string_hash(ext))
			{
			case string_hash(".vertspv"): return vk::ShaderStageFlagBits::eVertex                ;
			case string_hash(".geomspv"): return vk::ShaderStageFlagBits::eGeometry              ;
			case string_hash(".tesespv"): return vk::ShaderStageFlagBits::eTessellationEvaluation;
			case string_hash(".tescspv"): return vk::ShaderStageFlagBits::eTessellationControl   ;
			case string_hash(".fragspv"): return vk::ShaderStageFlagBits::eFragment              ;
			case string_hash(".compspv"): return vk::ShaderStageFlagBits::eCompute               ;
			default:                      return vk::ShaderStageFlagBits::eAll;
			}
		}(filepath.GetExtension());
		program->Load(shader_enum, {},stringify.str());
		
		return std::move(program);
	}
}
