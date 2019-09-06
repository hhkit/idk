#include "pch.h"
#include "VulkanGlslLoader.h"
#include <glslang/public/ShaderLang.h>
#include <glslang/GlslangToSpv.h>
#include <glslang/ResourceLimits.h>
#include <filesystem>
#include <sstream>

#include <vkn/VulkanWin32GraphicsSystem.h>
#include <util/string_hash.h>
#include <vkn/utils/utils.h>
namespace idk::vkn
{
	class GlslToSpirv
	{
		EShLanguage GetStage(string_view ext)
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
		std::vector<unsigned int> spirv(string_view glsl, string_view extension)
		{
			std::vector<unsigned int> spirv_out;
			auto stage = GetStage(extension);
			glslang::InitializeProcess();
			glslang::TShader shader(stage);
			shader.setEnvClient(glslang::EShClient::EShClientVulkan, glslang::EShTargetClientVersion::EShTargetVulkan_1_1);
			shader.setEnvInput(glslang::EShSource::EShSourceGlsl, stage, glslang::EShClient::EShClientVulkan, 450);
			shader.setEnvTarget(glslang::EShTargetLanguage::EShTargetSpv, glslang::EShTargetLanguageVersion::EShTargetSpv_1_1);
			auto tmp = glsl.data();
			shader.setStrings(&tmp, 1);
			TBuiltInResource dafuq = glslang::DefaultTBuiltInResource;
			if(!shader.parse(&dafuq, 450, true, EShMessages::EShMsgDefault))
				hlp::cerr()<<"Shader Error: " << shader.getInfoLog()<<"\n";
			glslang::GlslangToSpv(*shader.getIntermediate(), spirv_out);
			auto& sys = Core::GetSystem<VulkanWin32GraphicsSystem>();
			string_view sv{ r_cast<char*>(spirv_out.data()),spirv_out.size() * sizeof(spirv_out[0]) };
			glslang::FinalizeProcess();

			return spirv_out;
		}
	};

	string FileName(const FileHandle& path_to_resource)
	{
		//TODO actually get the file name
		return string(path_to_resource.GetFullPath());
	}
	FileResources VulkanGlslLoader::Create(FileHandle path_to_resource)
	{
		auto cmd = ("..\\tools\\glslc.exe " + FileName(path_to_resource) + " -o \"" + FileName(path_to_resource) + "spv\"");
		system(cmd.c_str());
		auto stage = (path_to_resource.GetExtension() == ".frag") ? EShLangFragment : EShLangVertex;
		
		return FileResources{};
	}

}