#include "pch.h"
#include <vkn/utils/utils.h>
#include <util/string_hash.h>

#include <glslang/GlslangToSpv.h>
#include <glslang/ResourceLimits.h>
#include "GlslToSpirv.h"
namespace idk::vkn
{

EShLanguage GlslToSpirv::GetStage(string_view ext)
{
	EShLanguage result{};
	switch (string_hash(ext))
	{
	case string_hash(".frag"):
		result = EShLangFragment;
		break;
	case string_hash(".vert"):
		result = EShLangVertex;
		break;
	case string_hash(".geom"):
		result = EShLangGeometry;
		break;
	case string_hash(".tesc"):
		result = EShLangTessControl;
		break;
	case string_hash(".tese"):
		result = EShLangTessEvaluation;
		break;
	default:
		break;
	}
	return result;
}

EShLanguage GlslToSpirv::ConvertStage(vk::ShaderStageFlagBits stage)
{
	static hash_table< vk::ShaderStageFlagBits, EShLanguage> conv
	{
		{ vk::ShaderStageFlagBits::eVertex,EShLangVertex },
	{ vk::ShaderStageFlagBits::eFragment,EShLangFragment },
	{ vk::ShaderStageFlagBits::eGeometry,EShLangGeometry },
	{ vk::ShaderStageFlagBits::eTessellationControl,EShLangTessControl },
	{ vk::ShaderStageFlagBits::eTessellationEvaluation,EShLangTessEvaluation },
	};
	auto itr = conv.find(stage);
	return itr->second;
}

std::optional<std::vector<unsigned int>> GlslToSpirv::spirv(string_view glsl, vk::ShaderStageFlagBits v_stage)
{
	std::optional<std::vector<unsigned int>> spirv_out;
	auto stage = ConvertStage(v_stage);
	glslang::InitializeProcess();
	glslang::TShader shader(stage);
	shader.setEnvClient(glslang::EShClient::EShClientVulkan, glslang::EShTargetClientVersion::EShTargetVulkan_1_0);
	shader.setEnvInput(glslang::EShSource::EShSourceGlsl, stage, glslang::EShClient::EShClientVulkan, 450);
	shader.setEnvTarget(glslang::EShTargetLanguage::EShTargetSpv, glslang::EShTargetLanguageVersion::EShTargetSpv_1_0);
	auto tmp = glsl.data();
	shader.setStrings(&tmp, 1);
	TBuiltInResource dafuq = glslang::DefaultTBuiltInResource;
	if (!shader.parse(&dafuq, 450, true, EShMessages::EShMsgVulkanRules))
		hlp::cerr() << "Shader Compilation Error: " << shader.getInfoLog() << "\n";
	else
	{
		glslang::SpvOptions opt{};
		opt.validate = true;
		spirv_out = std::vector<unsigned int>{};
		glslang::GlslangToSpv(*shader.getIntermediate(), *spirv_out);
	}
	glslang::FinalizeProcess();

	return spirv_out;
}

}