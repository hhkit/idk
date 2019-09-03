#pragma once
#include <idk.h>
#include <vulkan/vulkan.hpp>
#include <gfx/ShaderProgram.h>
namespace idk::vkn
{
	class ShaderModule :public ShaderProgram
	{
	public:
		void Load(vk::ShaderStageFlagBits single_stage, string_view byte_code);
		vk::ShaderStageFlagBits Stage()const { return stage; }
		vk::ShaderModule        Module()const { return *module; }

		bool NeedUpdate()const { return s_cast<bool>(back_module); }
		void Update() { if (NeedUpdate()) { module = std::move(back_module);  back_module.reset(); } }
	private:
		vk::ShaderStageFlagBits stage;
		vk::UniqueShaderModule module;
		vk::UniqueShaderModule back_module;//To load into, will moved into module when no buffers are using it
	};
}