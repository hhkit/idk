#include "pch.h"
#include "VulkanGlslLoader.h"
#include <filesystem>
#include <core/Core.h>
#include <res/MetaBundle.h>
#include <vkn/ShaderModule.h>
#include <vkn/VulkanWin32GraphicsSystem.h>
#include <vkn/utils/GlslToSpirv.h>
#include <util/ioutils.h>

namespace idk::vkn
{
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


	string FileName(const PathHandle& path_to_resource)
	{
		//TODO actually get the file name
		return string(path_to_resource.GetFullPath());
	}

	ResourceBundle VulkanGlslLoader::LoadFile(PathHandle path_to_resource, const MetaBundle& meta)
	{
		auto m = meta.FetchMeta<ShaderProgram>();
		auto program = m ? Core::GetResourceManager().LoaderEmplaceResource<ShaderModule>(m->guid)
			: Core::GetResourceManager().LoaderEmplaceResource<ShaderModule>();
		auto& filepath = path_to_resource;
		auto shader_stream = filepath.Open(FS_PERMISSIONS::READ);
		string val = stringify(shader_stream);

		auto shader_enum = GetShaderType(filepath.GetExtension());
		auto spirv = GlslToSpirv::spirv(val, shader_enum);
		if (spirv)
			program->Load(shader_enum, {}, string_view{ r_cast<const char*>(spirv->data()),hlp::buffer_size(*spirv) });
		return program;
	}

	ResourceBundle VulkanSpvLoader::LoadFile(PathHandle path, RscHandle<ShaderModule> program)
	{
		auto pprogram = s_cast<RscHandle<ShaderProgram>>(program);
		auto& filepath = path;
		auto name = path.GetFileName();
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
		return program;
	}
	
	ResourceBundle VulkanSpvLoader::LoadFile(PathHandle handle, const MetaBundle& meta)
	{
		auto m = meta.FetchMeta<ShaderProgram>();
		auto program = m
			? Core::GetResourceManager().LoaderEmplaceResource<ShaderModule>(m->guid)
			: Core::GetResourceManager().LoaderEmplaceResource<ShaderModule>();
		return LoadFile(handle, program);
	}

	}