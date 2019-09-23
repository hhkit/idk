#include "pch.h"
#include "VulkanGlslLoader.h"
#include <filesystem>
#include <sstream>
#include <vkn/ShaderModule.h>
#include <vkn/VulkanWin32GraphicsSystem.h>
#include <vkn/utils/GlslToSpirv.h>
namespace idk::vkn
{


	string FileName(const PathHandle& path_to_resource)
	{
		//TODO actually get the file name
		return string(path_to_resource.GetFullPath());
	}

	vk::ShaderStageFlagBits GetShaderType(string_view ext)
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

	}

	FileResources VulkanGlslLoader::Create(PathHandle path_to_resource)
	{
		auto program = Core::GetResourceManager().Emplace<ShaderModule>();
		auto pprogram = s_cast<RscHandle<ShaderProgram>>(program);

		auto& filepath = path_to_resource;
		auto shader_stream = filepath.Open(FS_PERMISSIONS::READ, true);
		std::stringstream stringify;
		stringify << shader_stream.rdbuf();
		string val = stringify.str();

		auto shader_enum = GetShaderType(filepath.GetExtension());
		auto spirv =GlslToSpirv::spirv(val, shader_enum);
		if (spirv)
		{
			//auto out_path = string { path_to_resource.GetMountPath()
			//} +string{ ".spv" };
			//PathHandle out_file = out_path;
			//auto strm = out_file.Open(FS_PERMISSIONS::WRITE, true);
			//strm << string_view{ r_cast<const char*>(std::data(*spirv)),hlp::buffer_size(*spirv) };
			//strm.close();
			//out_file = out_path;
			//Core::GetSystem<FileSystem>().Update();
			//out_file = out_path;
			//result = std::move(Core::GetResourceManager().LoadFile(out_file));
			program->Load(shader_enum, {}, string_view{ r_cast<const char*>((*spirv).data()),hlp::buffer_size(*spirv) });
		}

		FileResources result;
		result.resources.emplace_back(pprogram);
		return std::move(result);
	}

	FileResources VulkanSpvLoader::Create(PathHandle path_to_resource)
	{
		auto program = Core::GetResourceManager().Emplace<ShaderModule>();
		auto pprogram = s_cast<RscHandle<ShaderProgram>>(program);
		auto& filepath = path_to_resource;
		auto name = path_to_resource.GetFileName();
		auto last = name.find_last_of('.');
		auto just_name = name.substr(0, last);
		auto last2nd = just_name.find_last_of('.');
		auto ext = name.substr(last2nd, last - last2nd);
		auto shader_enum = GetShaderType(ext);
		auto shader_stream = filepath.Open(FS_PERMISSIONS::READ, true);
		std::stringstream stringify;
		stringify << shader_stream.rdbuf();
		string val = stringify.str();

		program->Load(shader_enum, {}, val);
		FileResources result;
		result.resources.emplace_back(pprogram);
		return std::move(result);
	}

}