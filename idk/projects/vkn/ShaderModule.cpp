#include "pch.h"
#include "ShaderModule.h"
#include <vkn/VulkanWin32GraphicsSystem.h>
#include <spirv_cross/spirv_reflect.hpp>
#include <gfx/pipeline_config.h>
#include <vkn/GfxConverters.h>
namespace idk::vkn
{
	namespace spx = spirv_cross;
	uniform_layout_t::UniformStage StageToUniformStage(vk::ShaderStageFlagBits single_stage)
	{
		uniform_layout_t::UniformStage result;
		switch (single_stage)
		{
		case vk::ShaderStageFlagBits::eVertex:
			result = uniform_layout_t::UniformStage::eVertex;
			break;
		case vk::ShaderStageFlagBits::eFragment:
			result = uniform_layout_t::UniformStage::eFragment;
			break;
		default:
			throw std::runtime_error("Unexpected shader stage encountered.");
			break;
		}
		return result;
	}
void ShaderModule::Load(vk::ShaderStageFlagBits single_stage, vector<buffer_desc> descriptors, string_view byte_code)
{
	vector<uint32_t> buffer;
	buffer.resize(byte_code.size()/sizeof(uint32_t));
	std::memcpy(buffer.data(), byte_code.data(), byte_code.size());
	spx::CompilerReflection code_reflector{ buffer };// r_cast<const uint32_t*>(std::data(buffer)), byte_code.size() / sizeof(uint16_t)//};
	auto aaaa = code_reflector.compile();
	auto resources = code_reflector.get_shader_resources();
	auto& view = Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View();
	//type
	ubo_info.clear();
	for (auto& ub : resources.uniform_buffers)
	{
		UboInfo info;
		auto type = code_reflector.get_type(ub.type_id);
		info.binding = code_reflector.get_decoration(ub.id, spv::Decoration::DecorationBinding);
		info.set     = code_reflector.get_decoration(ub.id, spv::Decoration::DecorationDescriptorSet);
		info.stage   = StageToUniformStage(single_stage);
		info.size    = type.width;
		info.type    = vk::DescriptorType::eUniformBuffer;
		uint32_t i = 0;
		for (auto& member_type : type.member_types)
		{
			auto tmp = code_reflector.get_type(member_type);
			info.size += code_reflector.get_declared_struct_member_size(type,i);
		}
		ubo_info[ub.name]= std::move(info);
	}
	back_module = view.CreateShaderModule(byte_code);
	stage = single_stage;
	attrib_descriptions = std::move(descriptors);
}

const UboInfo& ShaderModule::GetLayout(string uniform_name)
{
	return ubo_info[uniform_name];
}

}