#include "pch.h"
#include "VulkanMaterial.h"
#include <vkn/ShaderModule.h>
#include <file/FileSystem.h>
#include <gfx/ShaderTemplate.h>
#include <fstream>
#include <sstream>
namespace idk::vkn
{
	bool VulkanMaterial::BuildShader(RscHandle<ShaderTemplate> lighting_model, string_view material_uniforms, string_view material_code)
	{
		Core::GetResourceManager().Free(RscHandle<ShaderProgram>{meta.compiled_shader_guid});
		auto prog = Core::GetResourceManager().Create<ShaderModule>(meta.compiled_shader_guid);
		auto tmp_filename = "assets/shader/tmp" + meta.compiled_shader_guid.operator idk::string() + ".frag";
		auto tmp_outfilename = tmp_filename+"spv";
		std::ofstream tmp{ tmp_filename };
		tmp << lighting_model->Instantiate(material_uniforms, material_code) << std::flush;
		tmp.close();
		auto cmd = ("..\\tools\\glslc.exe " + tmp_filename + " -o \"" + tmp_outfilename+"\"");
		auto exit_code = system(cmd.c_str());
		auto ret = exit_code == 0;
		if (!ret)
		{
			std::ifstream file{tmp_filename+"spv",std::ios::binary };
			std::stringstream strm;
			strm << file.rdbuf();
			prog->Load(vk::ShaderStageFlagBits::eFragment, {}, strm.str());

			std::filesystem::remove(std::filesystem::relative(tmp_outfilename));
		}
		std::filesystem::remove(std::filesystem::relative(tmp_filename));

		return ret;
	}
}
