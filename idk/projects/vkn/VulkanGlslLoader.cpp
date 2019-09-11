#include "pch.h"
#include "VulkanGlslLoader.h"
#include <filesystem>
#include <sstream>
#include <vkn/ShaderModule.h>
#include <vkn/VulkanWin32GraphicsSystem.h>
#include <vkn/utils/GlslToSpirv.h>
namespace idk::vkn
{
	constexpr auto replacer = R"(
#ifdef OGL
#define U_LAYOUT(SET, BIND) 
#define BLOCK(X) struct X
#endif
#ifdef VULKAN
#define U_LAYOUT(SET, BIND) layout(set = SET, binding = BIND) 
#define BLOCK(X) X
#endif
)";


	string FileName(const FileHandle& path_to_resource)
	{
		//TODO actually get the file name
		return string(path_to_resource.GetFullPath());
	}
	unique_ptr<ShaderProgram> VulkanGlslLoader::GenerateDefaultResource()
	{
		return unique_ptr<ShaderProgram>();
	}
	unique_ptr<ShaderProgram> VulkanGlslLoader::Create(FileHandle path_to_resource)
	{

		auto program = std::make_unique<ShaderModule>();
		auto& filepath = path_to_resource;
		auto shader_stream = filepath.Open(FS_PERMISSIONS::READ, true);
		std::stringstream stringify;
		stringify << shader_stream.rdbuf();
		string val = stringify.str();
		string shader_code = val;
		auto version_pos = shader_code.find("#version");
		auto version_end = shader_code.find("\n", version_pos);

		val =  shader_code.substr(version_pos) + "#define VULKAN\n"+ replacer+ shader_code.substr(version_end);

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
		auto spirv =GlslToSpirv::spirv(val, shader_enum);
		if(spirv)
			program->Load(shader_enum, {}, string_view{ r_cast<const char*>((*spirv).data()),hlp::buffer_size(*spirv) });

		return std::move(program);
	}

}