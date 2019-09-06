#include "pch.h"
#include "VulkanMaterial.h"
#include <vkn/ShaderModule.h>
#include <file/FileSystem.h>
#include <gfx/ShaderTemplate.h>
#include <fstream>
#include <sstream>
#include <vkn/utils/GlslToSpirv.h>
namespace idk::vkn
{
	bool VulkanMaterial::BuildShader(RscHandle<ShaderTemplate> lighting_model, string_view material_uniforms, string_view material_code)
	{
		bool ret = false;
		Core::GetResourceManager().Free(RscHandle<ShaderProgram>{meta.compiled_shader_guid});
		auto prog = Core::GetResourceManager().Create<ShaderModule>(meta.compiled_shader_guid);
		//std::ofstream tmp{ tmp_filename };
		//tmp << << std::flush;
		//tmp.close();
		auto glsl = lighting_model->Instantiate(material_uniforms, material_code);
		auto spirv = GlslToSpirv::spirv(glsl, vk::ShaderStageFlagBits::eFragment);
		ret = static_cast<bool>(spirv);
		if (ret)
		{

			prog->Load(vk::ShaderStageFlagBits::eFragment, {}, *spirv);

		}

		return ret;
	}
}
