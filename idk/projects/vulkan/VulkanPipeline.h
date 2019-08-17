#pragma once
#include <Vulkan.h>
#include <VulkanDetail.h>
namespace idk
{

struct VulkGfxPipeline
{
	using Vulkan_t = vgfx::VulkanDetail;
	vk::UniquePipelineLayout m_pipelinelayout;
	vk::UniquePipeline       m_pipeline;
	enum FillType
	{
		eFill
		,eLine
	};
	struct config_t
	{
		string_view frag_shader{};
		string_view vert_shader{};
		std::optional<ivec2> screen_size{};
		FillType fill_type = eFill;
	};
	struct uniform_info
	{
		struct layout
		{

		};
		vector<layout> layouts;
		
	};
	void Create(config_t const& config, Vulkan_t& vulkan)
	{
		auto& m_device = vulkan.Device();
		auto& dispatcher = vulkan.Dispatcher();
		auto& m_renderpass = GetRenderpass(config, vulkan);
		auto [stageCreateInfo, stage_rsc] = GetShaderStageInfo(config);

		auto binding_desc = GetVtxBindingInfo(config);
		auto attr_desc = GetVtxAttribInfo(config);

		vk::PipelineVertexInputStateCreateInfo vertexInputInfo{
			vk::PipelineVertexInputStateCreateFlags{}
			,ArrCount(binding_desc)                         //vertexBindingDescriptionCount   
			,ArrData(binding_desc)                          //pVertexBindingDescriptions      
			,ArrCount(attr_desc)                            //vertexAttributeDescriptionCount 
			,ArrData(attr_desc)                             //pVertexAttributeDescriptions
		};
		 auto inputAssembly = GetAssemblyInfo(config);

		auto viewport= GetViewport(config,vulkan);
		auto scissor=GetScissor(config,vulkan);

		vk::PipelineViewportStateCreateInfo viewportState
		{
			vk::PipelineViewportStateCreateFlags{}
			,1,&viewport
			,1,&scissor
		};

		auto rasterizer = GetRasterizerInfo(config);

		auto multisampling = GetMultisampleInfo(config);

		//VkPipelineDepthStencilStateCreateInfo //For depth/stencil buffers; ignored for now

		auto colorBlending = GetColorBlendConfig(config);


		auto dynamicStates = GetDynamicStates(config);

		vk::PipelineDynamicStateCreateInfo dynamicState
		{
			vk::PipelineDynamicStateCreateFlags{}
			,ArrCount(dynamicStates)            //dynamicStateCount 
			,ArrData (dynamicStates)//pDynamicStates    
		};
		//For uniforms
		auto pipelineLayoutInfo = GetLayoutInfo(config);;
		auto m_pipelinelayout = m_device->createPipelineLayoutUnique(pipelineLayoutInfo, nullptr, dispatcher);
		vk::GraphicsPipelineCreateInfo pipelineInfo
		{
			vk::PipelineCreateFlags{}
			,ArrCount(stageCreateInfo),ArrData(stageCreateInfo)
			,&vertexInputInfo
			,&inputAssembly
			,nullptr
			,&viewportState
			,&rasterizer
			,&multisampling
			,nullptr
			,&colorBlending
			,nullptr
			,*m_pipelinelayout
			,*m_renderpass
			,0
		};
		m_pipeline = m_device->createGraphicsPipelineUnique({}, pipelineInfo, nullptr, dispatcher);
	}
	void Reset()
	{
		m_pipelinelayout.reset();
		m_pipeline.reset();
	}
	void Bind(const vk::CommandBuffer& cmd_buffer, Vulkan_t& vulkan)const
	{
		cmd_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *m_pipeline, vulkan.Dispatcher());
	}
	//Vulkan_t is necessary cause it needs to get the descriptors from the pool
	vector<vk::DescriptorSet> GetUniformDescriptors(Vulkan_t& vulkan,const uniform_info& uniform)
	{
		return {};
	}
	//AllocUniformBuffers is meant to write to a host master(non-vulkan) uniform buffer and get the offset.
	//The non-vulkan buffer will be transfered after the commands are done queueing.
	uint32_t AllocUniformBuffers(Vulkan_t& vulkan, const uniform_info& uniform);
	void BindUniformDescriptions(const vk::CommandBuffer& cmd_buffer, Vulkan_t& vulkan, const uniform_info& uniform)
	{
		//AllocUniformBuffers is meant to write to a host master uniform buffer and get the offset.
		//device master uniform buffer will be updated once all draw calls are queued.
		uint32_t start = AllocUniformBuffers(vulkan, uniform);
		//We can update the buffer and immediately queue a transfer command because we are expecting
		//the command submit order to be (Send Host Uniform Buffer to Device Uniform Buffer) -> (The render pass commands)
		cmd_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *m_pipelinelayout, start, GetUniformDescriptors(vulkan,uniform), nullptr, vulkan.Dispatcher());
	}
private:
	vk::PolygonMode GetPolygonMode(const config_t& config)const
	{
		static const hash_table<FillType, vk::PolygonMode> lookup
		{
			{eFill,vk::PolygonMode::eFill},
			{eLine,vk::PolygonMode::eLine},
		};
		return lookup.find(config.fill_type)->second;
	}
	vk::PipelineInputAssemblyStateCreateInfo    GetAssemblyInfo(const config_t& config)const
	{
		return vk::PipelineInputAssemblyStateCreateInfo{
			vk::PipelineInputAssemblyStateCreateFlags{}
			,vk::PrimitiveTopology::eTriangleList
			,VK_FALSE                              //Set to true to allow strips to be restarted with special indices 0xFFFF or 0xFFFFFFFF
		};
	}
	vector<vk::VertexInputAttributeDescription> GetVtxAttribInfo(const config_t& config)const;
	vector<vk::VertexInputBindingDescription> GetVtxBindingInfo(const config_t& config)const;
	std::pair<
		vector<vk::PipelineShaderStageCreateInfo>,
		vector<vk::UniqueShaderModule>
	>
		GetShaderStageInfo(const config_t& config)const
	{
		std::pair<
			vector<vk::PipelineShaderStageCreateInfo>,
			vector<vk::UniqueShaderModule>
		> result;
		auto& [info, rsc] = result;

		auto vert = config.vert_shader;//GetBinaryFile("shaders/vertex.vert.spv");
		auto frag = config.frag_shader;//GetBinaryFile("shaders/fragment.frag.spv");

		auto& fragModule = rsc.emplace_back(createShaderModule(frag));
		auto& vertModule = rsc.emplace_back(createShaderModule(vert));

		const char* entryPoint = "main";

		vk::PipelineShaderStageCreateInfo fragShaderStageInfo
		{
			vk::PipelineShaderStageCreateFlags{},
			vk::ShaderStageFlagBits::eFragment,
			*fragModule,
			entryPoint,
			nullptr
		};

		vk::PipelineShaderStageCreateInfo vertShaderStageInfo
		{
			vk::PipelineShaderStageCreateFlags{},
			vk::ShaderStageFlagBits::eVertex,
			*vertModule,
			entryPoint,
			nullptr
		};
		//vk::PipelineShaderStageCreateInfo stageCreateInfo[] = { vertShaderStageInfo,fragShaderStageInfo };
		info.emplace_back(fragShaderStageInfo);
		info.emplace_back(vertShaderStageInfo);
		return result;
	}

	vk::Viewport GetViewport(const config_t& config,Vulkan_t& vulkan)const
	{
		auto sc = vulkan.Swapchain().extent;
		ivec2 screen_size = (config.screen_size) ? *config.screen_size : ivec2{ s_cast<int>(sc.width),s_cast<int>(sc.height) };
		return vk::Viewport
		{
			0.0f,0.0f, //x,y
			(float)screen_size.x, (float)screen_size.y,
			0.0f,1.0f // min/max depth
		};
	}
	vk::Rect2D GetScissor(const config_t& config, Vulkan_t& vulkan)const
	{
		auto sc = vulkan.Swapchain().extent;
		ivec2 screen_size = (config.screen_size) ? *config.screen_size : ivec2{ s_cast<int>(sc.width),s_cast<int>(sc.height) };
		return vk::Rect2D{
			{ 0,0 },
			{ s_cast<uint32_t>(screen_size.x), s_cast<uint32_t>(screen_size.y)}
		};
	}
	vk::PipelineRasterizationStateCreateInfo GetRasterizerInfo(const config_t& config) const
	{
		return vk::PipelineRasterizationStateCreateInfo
		{
			vk::PipelineRasterizationStateCreateFlags{}
				, VK_FALSE //depthClampEnable VK_FALSE discards fragments that are beyond the depth range, VK_TRUE clamps it instead.
				, VK_FALSE //If rasterizerDiscardEnable is set to VK_TRUE, then geometry never passes through the rasterizer stage. This basically disables any output to the framebuffer.
				, GetPolygonMode(config) //Any other mode requires enabling a GPU feature
				, vk::CullModeFlagBits::eFront
				, vk::FrontFace::eClockwise
				, VK_FALSE
				, 0.0f
				, 0.0f
				, 0.0f
				, 1.0f //Any lines thicker than 1.0f requires wideLines gpu feature.
		};
	}
	vk::PipelineMultisampleStateCreateInfo GetMultisampleInfo(const config_t& config)const
	{

		return vk::PipelineMultisampleStateCreateInfo{
			vk::PipelineMultisampleStateCreateFlags{}
			,vk::SampleCountFlagBits::e1 //rasterizationSamples  
			,VK_FALSE					 //sampleShadingEnable   
			,1.0f						 //minSampleShading      // Optional
			,nullptr					 //pSampleMask           // Optional
			,VK_FALSE					 //alphaToCoverageEnable // Optional
			,VK_FALSE					 //alphaToOneEnable      // Optional
		};
	}
	vector<vk::PipelineColorBlendAttachmentState > GetColorBlendAttachments(const config_t& config)const
	{
		return {
			vk::PipelineColorBlendAttachmentState
		{
			/*blendEnable         */VK_FALSE
			/*srcColorBlendFactor */,vk::BlendFactor::eOne	//optional
			/*dstColorBlendFactor */,vk::BlendFactor::eZero	//optional
			/*colorBlendOp        */,vk::BlendOp::eAdd		//optional
			/*srcAlphaBlendFactor */,vk::BlendFactor::eOne	//optional
			/*dstAlphaBlendFactor */,vk::BlendFactor::eZero	//optional
			/*alphaBlendOp        */,vk::BlendOp::eAdd      //optional
			/*colorWriteMask      */,vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
		}
		};

		/*
		//How the values are used are like this:
		if (blendEnable) {
		finalColor.rgb = (srcColorBlendFactor * newColor.rgb) <colorBlendOp> (dstColorBlendFactor * oldColor.rgb);
		finalColor.a = (srcAlphaBlendFactor * newColor.a) <alphaBlendOp> (dstAlphaBlendFactor * oldColor.a);
		} else {
		finalColor = newColor;
		}

		finalColor = finalColor & colorWriteMask;
		*/
	}
	vk::PipelineColorBlendStateCreateInfo GetColorBlendConfig(const config_t& config)const
	{
		//Per frame buffer
		auto colorBlendAttachments = GetColorBlendAttachments(config);
		return vk::PipelineColorBlendStateCreateInfo
		{
			vk::PipelineColorBlendStateCreateFlags{}
			,VK_FALSE                           //logicOpEnable   
			,vk::LogicOp::eCopy	                //logicOp         
			,ArrCount(colorBlendAttachments)    //attachmentCount 
			,ArrData(colorBlendAttachments)     //pAttachments   
			,{ 0.0,0.0f,0.0f,0.0f }
		};
	}
	vector<vk::DynamicState> GetDynamicStates(const config_t& config)const
	{
		return{
				vk::DynamicState::eViewport,
				vk::DynamicState::eLineWidth
		};
	}
	vk::PipelineLayoutCreateInfo GetLayoutInfo(const config_t& config)const
	{

		vk::PipelineLayoutCreateInfo pipelineLayoutInfo =
		{
			vk::PipelineLayoutCreateFlags{}
			, 1		  //setLayoutCount         
			, &*m_descriptorsetlayout //pSetLayouts            
			, 0		  //pushConstantRangeCount 
			, nullptr //pPushConstantRanges    
		};
	}
	vk::UniqueRenderPass& GetRenderpass(const config_t& config, vgfx::VulkanDetail& vulkan)
	{
		return vulkan.Renderpass();
	}
}; 
}