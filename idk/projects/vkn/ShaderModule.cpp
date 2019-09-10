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
	void extract_info(const vector<unsigned int>& buffer, hash_table<string, UboInfo>& ubo_info, vk::ShaderStageFlagBits single_stage)
	{
		spx::CompilerReflection code_reflector{ buffer };// r_cast<const uint32_t*>(std::data(buffer)), byte_code.size() / sizeof(uint16_t)//};
		//code_reflector.build_combined_image_samplers();
		auto aaaa = code_reflector.compile();
		auto resources = code_reflector.get_shader_resources();
		//type
		
		ubo_info.clear();
		for (auto& ub : resources.uniform_buffers)
		{
			UboInfo info;
			auto type = code_reflector.get_type(ub.type_id);
			info.binding = code_reflector.get_decoration(ub.id, spv::Decoration::DecorationBinding);
			info.set = code_reflector.get_decoration(ub.id, spv::Decoration::DecorationDescriptorSet);
			info.stage = StageToUniformStage(single_stage);
			info.size = type.width;
			info.type = uniform_layout_t::UniformType::eBuffer;
			uint32_t i = 0;
			for (auto& member_type : type.member_types)
			{
				auto tmp = code_reflector.get_type(member_type);
				info.size += s_cast<uint32_t>(code_reflector.get_declared_struct_member_size(type, i));
			}
			ubo_info[ub.name] = std::move(info);
		}
		auto& cs = code_reflector.get_combined_image_samplers();

		for (auto& ub : resources.sampled_images)
		{
			UboInfo info;
			auto type = code_reflector.get_type(ub.type_id);
			info.binding = code_reflector.get_decoration(ub.id, spv::Decoration::DecorationBinding);
			info.set = code_reflector.get_decoration(ub.id, spv::Decoration::DecorationDescriptorSet);
			info.stage = StageToUniformStage(single_stage);
			info.size = type.width;
			info.type = uniform_layout_t::UniformType::eSampler;
			uint32_t i = 0;
			for (auto& member_type : type.member_types)
			{
				auto tmp = code_reflector.get_type(member_type);
				info.size += s_cast<uint32_t>(code_reflector.get_declared_struct_member_size(type, i));
			}
			ubo_info[ub.name] = std::move(info);
		}
		//for (auto& ub : cs)
		//{
		//	UboInfo info;
		//	
		//	info.binding = code_reflector.get_decoration(ub.combined_id, spv::Decoration::DecorationBinding);
		//	info.binding = code_reflector.get_decoration(ub.sampler_id, spv::Decoration::DecorationBinding);
		//	info.set = code_reflector.get_decoration(ub.combined_id, spv::Decoration::DecorationDescriptorSet);
		//	info.set = code_reflector.get_decoration(ub.sampler_id, spv::Decoration::DecorationDescriptorSet);
		//	info.stage = StageToUniformStage(single_stage);
		//	info.type = vk::DescriptorType::eCombinedImageSampler;
		//	uint32_t i = 0;
		//	ubo_info[code_reflector.get_name(ub.combined_id)] = std::move(info);
		//}
	}
void ShaderModule::Load(vk::ShaderStageFlagBits single_stage, vector<buffer_desc> descriptors, const vector<unsigned int>& buffer)
{
	string_view byte_code{r_cast<const char*>(buffer.data()),hlp::buffer_size(buffer)};
	auto& view = Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View();
	extract_info(buffer, ubo_info, single_stage);
	back_module = view.CreateShaderModule(byte_code);
	stage = single_stage;
	attrib_descriptions = std::move(descriptors);
}
void ShaderModule::Load(vk::ShaderStageFlagBits single_stage, vector<buffer_desc> descriptors, string_view byte_code)
{
	vector<unsigned int> buffer;
	buffer.resize(byte_code.size()/sizeof(uint32_t));
	std::memcpy(buffer.data(), byte_code.data(), byte_code.size());
	Load(single_stage, descriptors, buffer);
}

const UboInfo& ShaderModule::GetLayout(string uniform_name)
{
	return ubo_info[uniform_name];
}

}