#include "pch.h"
#include "ShaderModule.h"
#include <vkn/VulkanWin32GraphicsSystem.h>
#include <spirv_cross/spirv_reflect.hpp>

namespace idk::vkn
{
	namespace spx = spirv_cross;
	struct UboInfo
	{
		spx::SPIRType type;
		string name;
		uint32_t binding, set;
	};
void ShaderModule::Load(vk::ShaderStageFlagBits single_stage, vector<buffer_desc> descriptors, string_view byte_code)
{
	vector<uint32_t> buffer;
	buffer.resize(byte_code.size()/sizeof(uint32_t));
	std::memcpy(buffer.data(), byte_code.data(), byte_code.size());
	spx::CompilerReflection code_reflector{ buffer };// r_cast<const uint32_t*>(std::data(buffer)), byte_code.size() / sizeof(uint16_t)//};
	auto aaaa = code_reflector.compile();
	auto resources = code_reflector.get_shader_resources();
	for (auto& ub : resources.uniform_buffers)
	{
		UboInfo info;
		info.type = code_reflector.get_type(ub.type_id);
		info.binding = code_reflector.get_decoration(ub.id, spv::Decoration::DecorationBinding);
		info.set     = code_reflector.get_decoration(ub.id, spv::Decoration::DecorationDescriptorSet);
	}
	back_module = Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View().CreateShaderModule(byte_code);
	stage = single_stage;
	attrib_descriptions = std::move(descriptors);
}

}