#pragma once
#include <vkn/VulkanState.h>
#include <vkn/VulkanView.h>
#include <gfx/pipeline_config.h>
#include <gfx/uniform_info.h>

namespace idk::vkn
{
	struct VulkanPipeline
	{
		using Vulkan_t     = VulkanView;
		using config_t     = pipeline_config;

		vk::UniquePipelineLayout pipelinelayout{};
		vk::UniquePipeline       pipeline{};

	   hash_table<uint32_t,vk::UniqueDescriptorSetLayout> uniform_layouts{};

	
	   void Create(config_t const& config, vector<vk::PipelineShaderStageCreateInfo> info, Vulkan_t& vulkan);
	   void Create(config_t const& config, vector<std::pair<vk::ShaderStageFlagBits,vk::ShaderModule>> shader_modules, Vulkan_t& vulkan);
	   void Create(config_t const& config, Vulkan_t& vulkan);
		void Reset();
		void Bind(const vk::CommandBuffer& cmd_buffer, Vulkan_t& vulkan)const;
		//Unused for now. If we don't switch to this by Milestone 1, we probably won't need it.
		//bool HasUniforms(const uniform_info& uni)const;
		//void BindUniformDescriptions(const vk::CommandBuffer& cmd_buffer, Vulkan_t& vulkan, const uniform_info& uniform);
	private:
		vk::PolygonMode GetPolygonMode(const config_t& config)const;
		vk::PipelineInputAssemblyStateCreateInfo    GetAssemblyInfo(const config_t& config)const;
		vector<vk::VertexInputAttributeDescription> GetVtxAttribInfo(const config_t& config)const;
		vector<vk::VertexInputBindingDescription>   GetVtxBindingInfo(const config_t& config)const;
		std::pair<
			vector<vk::PipelineShaderStageCreateInfo>,
			vector<vk::UniqueShaderModule>
		>
			GetShaderStageInfo(const config_t& config, Vulkan_t& vulkan)const;

		vk::Viewport GetViewport(const config_t& config, Vulkan_t& vulkan)const;
		vk::Rect2D GetScissor(const config_t& config, Vulkan_t& vulkan)const;
		vk::PipelineRasterizationStateCreateInfo GetRasterizerInfo(const config_t& config) const;
		vk::PipelineMultisampleStateCreateInfo GetMultisampleInfo(const config_t& config)const;
		vector<vk::PipelineColorBlendAttachmentState > GetColorBlendAttachments(const config_t& config)const;
		std::pair<vk::PipelineColorBlendStateCreateInfo, vector<vk::PipelineColorBlendAttachmentState >> GetColorBlendConfig(const config_t& config)const;
		vector<vk::DynamicState> GetDynamicStates(const config_t& config)const;
		std::pair<vk::PipelineLayoutCreateInfo,vector< vk::DescriptorSetLayout>> GetLayoutInfo(const config_t& config)const;
		void CreateUniformDescriptors(Vulkan_t& vulkan, const config_t& config);
		vk::UniqueRenderPass& GetRenderpass(const config_t& config, VulkanView& vulkan);

		// Vulkan_t is necessary cause it needs to get the descriptors from the pool
		vector<vk::DescriptorSet> GetUniformDescriptors(Vulkan_t& vulkan);

		//Unused for now. If we don't switch to this by Milestone 1, we probably won't need it.
		// AllocUniformBuffers is meant to write to a host master(non-vulkan) uniform buffer and get the offset.
		// The non-vulkan buffer will be transfered after the commands are done queueing.
		//uint32_t AllocUniformBuffers(Vulkan_t& vulkan, const uniform_info& uniform);
	}; 
}