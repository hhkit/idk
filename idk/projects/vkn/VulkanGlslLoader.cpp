#include "pch.h"
#include "VulkanGlslLoader.h"
#include <filesystem>
#include <core/Core.h>
#include <vkn/ShaderModule.h>
#include <vkn/VulkanWin32GraphicsSystem.h>
#include <vkn/utils/GlslToSpirv.h>
#include <util/ioutils.h>

namespace idk::vkn
{


	string FileName(const PathHandle& path_to_resource)
	{
		//TODO actually get the file name
		return string(path_to_resource.GetFullPath());
	}
	unique_ptr<ShaderProgram> VulkanGlslFactory::GenerateDefaultResource()
	{
		return unique_ptr<ShaderProgram>();
	}

	ResourceBundle VulkanGlslLoader::LoadFile(PathHandle path_to_resource)
	{
		auto program = Core::GetResourceManager().LoaderEmplaceResource<ShaderModule>();
		auto& filepath = path_to_resource;
		auto shader_stream = filepath.Open(FS_PERMISSIONS::READ);
		string val = stringify(shader_stream);

		auto shader_enum = [](std::string_view ext)->vk::ShaderStageFlagBits
		{
			switch (string_hash(ext))
			{
			case string_hash(".vert"): return vk::ShaderStageFlagBits::eVertex;
			case string_hash(".geom"): return vk::ShaderStageFlagBits::eGeometry;
			case string_hash(".tese"): return vk::ShaderStageFlagBits::eTessellationEvaluation;
			case string_hash(".tesc"): return vk::ShaderStageFlagBits::eTessellationControl;
			case string_hash(".frag"): return vk::ShaderStageFlagBits::eFragment;
			case string_hash(".comp"): return vk::ShaderStageFlagBits::eCompute;
			default:                      return vk::ShaderStageFlagBits::eAll;
			}
		}(filepath.GetExtension());
		auto spirv = GlslToSpirv::spirv(val, shader_enum);
		if (spirv)
			program->Load(shader_enum, {}, string_view{ r_cast<const char*>((*spirv).data()),hlp::buffer_size(*spirv) });

		return std::move(program);
	}

}