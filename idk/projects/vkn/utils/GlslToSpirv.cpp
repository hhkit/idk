#include "pch.h"
#include <vulkan/vulkan.hpp>
#include <vkn/utils/utils.h>
#include <util/string_hash.h>
//#define ENABLE_OPT 0
//#include <glslang/GlslangToSpv.h>
//#include <glslang/ResourceLimits.h>
#include "GlslToSpirv.h"
#include <sstream>
//#include <glslang/SpvTools.h>
#include <shaderc/shaderc.hpp>
#include <gfx/ShaderIncluder.h>
#include <file/FileSystem.h>
#include <core/core.h>
#include <serialize/text.inl>
namespace idk::vkn
{
	constexpr auto replacer = R"(
#ifdef OGL
#define U_LAYOUT(SET, BIND) 
#define S_LAYOUT(SET, BIND) 
#define BLOCK(X) struct X
#endif
#ifdef VULKAN
#define U_LAYOUT(SET, BIND) layout(std140, set = SET, binding = BIND) 
#define S_LAYOUT(SET, BIND) layout(set = SET, binding = BIND) 
#define BLOCK(X) X
#endif
)";
	/*
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
}*/
shaderc_shader_kind ConvertStageSC(vk::ShaderStageFlagBits stage)
{
	static hash_table< vk::ShaderStageFlagBits, shaderc_shader_kind> conv
	{
		{ vk::ShaderStageFlagBits::eVertex            ,shaderc_shader_kind::shaderc_vertex_shader },
	{ vk::ShaderStageFlagBits::eFragment              ,shaderc_shader_kind::shaderc_fragment_shader },
	{ vk::ShaderStageFlagBits::eGeometry              ,shaderc_shader_kind::shaderc_geometry_shader },
	{ vk::ShaderStageFlagBits::eTessellationControl   ,shaderc_shader_kind::shaderc_tess_control_shader },
	{ vk::ShaderStageFlagBits::eTessellationEvaluation,shaderc_shader_kind::shaderc_tess_evaluation_shader },
	};
	auto itr = conv.find(stage);
	return itr->second;
}
void DoNothing();
string PreprocessGlsl(string glsl)
{
	string shader_code = glsl;

	shader_code = ProcessIncludes(shader_code);

	auto version_pos = shader_code.find("#version");
	auto version_end = shader_code.find("\n", version_pos);

	return shader_code.substr(0, version_end) + replacer + shader_code.substr(version_end, shader_code.size() - version_end);
}
std::optional<std::vector<unsigned int>> GlslToSpirv::spirv(string_view glsl, vk::ShaderStageFlagBits v_stage, string_view code_id)
{
	string val = static_cast<string>(glsl);
	//string shader_code = val;
	//
	//shader_code = ProcessIncludes(shader_code);
	//
	//auto version_pos = shader_code.find("#version");
	//auto version_end = shader_code.find("\n", version_pos);
	//
	//val = shader_code.substr(0, version_end) + replacer + shader_code.substr(version_end, shader_code.size() - version_end);
	std::optional<std::vector<unsigned int>> spirv_out;/*
	auto stage = ConvertStage(v_stage);
	glslang::InitializeProcess();
	glslang::TShader shader(stage);
	
	shader.setEnvClient(glslang::EShClient::EShClientVulkan, glslang::EShTargetClientVersion::EShTargetVulkan_1_0);
	shader.setEnvInput(glslang::EShSource::EShSourceGlsl, stage, glslang::EShClient::EShClientVulkan, 430);
	shader.setEnvTarget(glslang::EShTargetLanguage::EShTargetSpv, glslang::EShTargetLanguageVersion::EShTargetSpv_1_0);
	auto tmp = glsl.data();
	shader.setStrings(&tmp, 1);
	TBuiltInResource dafuq = glslang::DefaultTBuiltInResource;
	dafuq.limits.generalUniformIndexing = false;
	if (!shader.parse(&dafuq, 430, true, EShMessages::EShMsgVulkanRules))
		hlp::cerr() << "Shader Compilation Error: " << shader.getInfoLog() << "\n";
	else
	{
		shader.setEnvClient(glslang::EShClient::EShClientVulkan, glslang::EShTargetClientVersion::EShTargetVulkan_1_1);
		shader.setEnvInput(glslang::EShSource::EShSourceGlsl, stage, glslang::EShClient::EShClientVulkan, 430);
		shader.setEnvTarget(glslang::EShTargetLanguage::EShTargetSpv, glslang::EShTargetLanguageVersion::EShTargetSpv_1_0);
		glslang::SpvOptions opt{};
		opt.disassemble = true;
		opt.generateDebugInfo = true;
		opt.disableOptimizer = false;
		opt.validate = true;
		spirv_out = std::vector<unsigned int>{};
		spv::SpvBuildLogger logger;
		{
			glslang::GlslangToSpv(*shader.getIntermediate(), *spirv_out, &logger,&opt);
			int a{};
			++a;
		}
		std::stringstream test;
		glslang::SpirvToolsDisassemble(test, *spirv_out);
		string tmaaaa = test.str();
		auto a = tmaaaa.size();
	}

	glslang::FinalizeProcess();

	*/
	{
		shaderc::Compiler compiler;
		shaderc::CompileOptions opt;
		opt.SetTargetEnvironment(shaderc_target_env::shaderc_target_env_vulkan, 0);
		auto result = compiler.CompileGlslToSpv(val, ConvertStageSC(v_stage), code_id.data(), opt);
		if (result.GetCompilationStatus() == shaderc_compilation_status::shaderc_compilation_status_success)
			spirv_out = vector<unsigned int>{ result.begin(),result.end() };
		else
		{
			DoNothing();
			string filename = "/" + string{ code_id } + serialize_text(string_hash(glsl));
			auto err_m = result.GetErrorMessage(); 
			auto err_msg = err_m.c_str();
			LOG_TO(LogPool::GFX, "%s", err_msg);
			try
			{
				auto path = string{ Core::GetSystem<FileSystem>().GetAppDataDir() } +"/idk";
                if (!std::filesystem::exists(path.sv()))
					std::filesystem::create_directory(path.sv());
				path += "/shader_err";
				if (!std::filesystem::exists(path.sv()))
					std::filesystem::create_directory(path.sv());
				auto out_file = path + filename;
				std::ofstream out{ out_file };
				out << val;
				out << " /* Error Message: \n" << err_msg << "\n*/";
				out.close();
			}
			catch (...)
			{
				LOG_TO(LogPool::GFX, "Failed to output glsl [%s] that generated the above error.", filename.c_str());
			}
		}
	}
	return spirv_out;
}

}
