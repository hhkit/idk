#pragma once
#include <vkn/VulkanState.h>
#include <vkn/VulkanView.h>
#include <gfx/pipeline_config.h>
#include <gfx/uniform_info.h>

#include <vkn/ManagedVulkanObjs.h>

namespace idk::vkn
{
	class ShaderModule;
	struct VulkanPipeline
	{
		using Vulkan_t     = VulkanView;
		using config_t     = pipeline_config;

		VulkanRsc<vk::PipelineLayout> pipelinelayout{};
		VulkanRsc<vk::Pipeline>       pipeline{};
		vk::Pipeline Pipeline()const;
		vk::PipelineLayout PipelineLayout()const;
		
		//Do this before calling create, overrides config's render_pass_type
		void SetRenderPass(vk::RenderPass rp, bool has_depth_stencil);
		//If you're reusing VulkanPipeline and don't want the previous override of render_pass_type to carry over.
		//Call before create.
		void ClearRenderPass();

		hash_table<uint32_t, vk::UniqueDescriptorSetLayout> owned_uniform_layouts{};
		hash_table<uint32_t,vk::DescriptorSetLayout> uniform_layouts{};

		std::optional<uint32_t> GetBinding(uint32_t location)const;


		struct Options
		{
			struct DerivativeInfo
			{
				vk::Pipeline base{};
			};
			std::optional<DerivativeInfo> derive_from{};
			bool is_base_pipeline=false; //Can be derived from
			vector<vk::DynamicState> dynamic_states{vk::DynamicState::eViewport };
		};

	
		void Create(config_t const& config, vector<vk::PipelineShaderStageCreateInfo> info, Vulkan_t& vulkan, const Options& options = Options{});
		void Create(config_t const& config, vector<RscHandle<ShaderProgram>> shaders,Vulkan_t& vulkan, const Options& options = Options{});
		void Create(config_t const& config, vector<vk::PipelineShaderStageCreateInfo> info,hash_table<uint32_t,vk::DescriptorSetLayout> slayout,Vulkan_t& vulkan, const Options& options = Options{});
	   void Create(config_t const& config, vector<std::pair<vk::ShaderStageFlagBits,vk::ShaderModule>> shader_modules, Vulkan_t& vulkan, const Options& options = Options{});
	   void Create(config_t const& config, Vulkan_t& vulkan, const Options& options = Options{});
		void Reset();
		void Bind(const vk::CommandBuffer& cmd_buffer, Vulkan_t& vulkan)const;
		//Unused for now. If we don't switch to this by Milestone 1, we probably won't need it.
		//bool HasUniforms(const uniform_info& uni)const;
		//void BindUniformDescriptions(const vk::CommandBuffer& cmd_buffer, Vulkan_t& vulkan, const uniform_info& uniform);
	private:
		std::optional<vk::RenderPass> _render_pass;
		bool _has_depth_stencil = false;
		//location to binding
		hash_table<uint32_t, uint32_t> loc2bind;
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
		std::pair<vector<vk::PipelineColorBlendStateCreateInfo>, vector<vk::PipelineColorBlendAttachmentState >> GetColorBlendConfig(const config_t& config)const;
		vector<vk::DynamicState> GetDynamicStates(const config_t& config, const Options& options)const;
		std::pair<vk::PipelineLayoutCreateInfo,vector< vk::DescriptorSetLayout>> GetLayoutInfo(const config_t& config)const;
		vk::RenderPass GetRenderpass(const config_t& config, VulkanView& vulkan);

		// Vulkan_t is necessary cause it needs to get the descriptors from the pool
		vector<vk::DescriptorSet> GetUniformDescriptors(Vulkan_t& vulkan);

		//Unused for now. If we don't switch to this by Milestone 1, we probably won't need it.
		// AllocUniformBuffers is meant to write to a host master(non-vulkan) uniform buffer and get the offset.
		// The non-vulkan buffer will be transfered after the commands are done queueing.
		//uint32_t AllocUniformBuffers(Vulkan_t& vulkan, const uniform_info& uniform);
	}; 
}