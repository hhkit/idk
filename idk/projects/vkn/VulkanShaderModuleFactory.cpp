#include "pch.h"
#include <sstream>
#include <core/Core.h>
#include "VulkanShaderModuleFactory.h"
#include <vkn/ShaderModule.h>
#include <res/MetaBundle.h>
#include <util/ioutils.h>

namespace idk::vkn
{
	ResourceBundle VulkanShaderModuleLoader::LoadFile(PathHandle filepath)
	{
		auto program = Core::GetResourceManager().LoaderEmplaceResource<ShaderModule>();
		auto shader_stream = filepath.Open(FS_PERMISSIONS::READ, true);

		auto shader_enum = [](std::string_view ext)->vk::ShaderStageFlagBits
		{
			switch (string_hash(ext))
			{
			case string_hash(".vertspv"): return vk::ShaderStageFlagBits::eVertex;
			case string_hash(".geomspv"): return vk::ShaderStageFlagBits::eGeometry;
			case string_hash(".tesespv"): return vk::ShaderStageFlagBits::eTessellationEvaluation;
			case string_hash(".tescspv"): return vk::ShaderStageFlagBits::eTessellationControl;
			case string_hash(".fragspv"): return vk::ShaderStageFlagBits::eFragment;
			case string_hash(".compspv"): return vk::ShaderStageFlagBits::eCompute;
			default:                      return vk::ShaderStageFlagBits::eAll;
			}
		}(filepath.GetExtension());

		program->Load(shader_enum, {}, stringify(shader_stream));

		return std::move(program);
	}
	ResourceBundle VulkanShaderModuleLoader::LoadFile(PathHandle filepath, const MetaBundle& bundle)
	{
		auto program = Core::GetResourceManager().LoaderEmplaceResource<ShaderModule>(bundle.metadatas[0].guid);
		auto shader_stream = filepath.Open(FS_PERMISSIONS::READ, true);

		auto shader_enum = [](std::string_view ext)->vk::ShaderStageFlagBits
		{
			switch (string_hash(ext))
			{
			case string_hash(".vertspv"): return vk::ShaderStageFlagBits::eVertex;
			case string_hash(".geomspv"): return vk::ShaderStageFlagBits::eGeometry;
			case string_hash(".tesespv"): return vk::ShaderStageFlagBits::eTessellationEvaluation;
			case string_hash(".tescspv"): return vk::ShaderStageFlagBits::eTessellationControl;
			case string_hash(".fragspv"): return vk::ShaderStageFlagBits::eFragment;
			case string_hash(".compspv"): return vk::ShaderStageFlagBits::eCompute;
			default:                      return vk::ShaderStageFlagBits::eAll;
			}
		}(filepath.GetExtension());

		program->Load(shader_enum, {}, stringify(shader_stream));

		return std::move(program);
	}
	//unique_ptr<ShaderProgram> VulkanShaderModuleFactory::Create()
	//{
	//	return std::make_unique<ShaderModule>();;
	//}
}
