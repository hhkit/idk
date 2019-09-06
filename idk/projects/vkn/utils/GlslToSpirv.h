#pragma once
#include <idk.h>
#include <vector>
#include <glslang/public/ShaderLang.h>
namespace idk::vkn
{
class GlslToSpirv
{
public:
	static EShLanguage GetStage(string_view ext);
	static EShLanguage ConvertStage(vk::ShaderStageFlagBits stage);
	static std::optional<std::vector<unsigned int>> spirv(string_view glsl, vk::ShaderStageFlagBits v_stage);
};
}