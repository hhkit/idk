#include "pch.h"
#include "ShaderModule.h"
#include <vkn/VulkanWin32GraphicsSystem.h>

namespace idk::vkn
{

void ShaderModule::Load(vk::ShaderStageFlagBits single_stage, vector<buffer_desc> descriptors, string_view byte_code)
{

	back_module = Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View().CreateShaderModule(byte_code);
	stage = single_stage;
	attrib_descriptions = std::move(descriptors);
}

}