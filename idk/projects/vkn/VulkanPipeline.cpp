#include "pch.h"
#include "VulkanPipeline.h"
#include <vkn/GfxConverters.h>
#include <vkn/BufferHelpers.h>
namespace idk::vkn
{
	template<typename T, typename = void>
	struct is_container : std::false_type {};
	template<typename T>
	struct is_container <T, std::void_t<decltype(std::begin(std::declval<T>()))>> : std::true_type {};
	template<typename T>
	inline constexpr bool is_container_v = is_container<T>::value;

	//To be able to queue multiple generations.
	//Call lock_in when submitting the draw call to advance the set
	struct UboData2
	{
		struct UniformEntry
		{
			alignas(sizeof(vec4)) unsigned char data[sizeof(vec4)];
		};
		void SetLocation(const vec4& val, uint32_t loc)
		{
			if (locations.size() <= loc) locations.resize(s_cast<size_t>(loc) + 1);
			std::memcpy(locations.back()[loc].data, &val, sizeof(val));
		}
		void SetLocation(const vec3& val, uint32_t loc)
		{
			if (locations.size() <= loc) locations.resize(s_cast<size_t>(loc) + 1);
			std::memcpy(locations.back()[loc].data, &val, sizeof(val));
		}
		void SetLocation(const vec2& val, uint32_t loc)
		{
			if (locations.size() <= loc) locations.resize(s_cast<size_t>(loc) + 1);
			std::memcpy(locations.back()[loc].data, &val, sizeof(val));
		}
		vector<hash_table<uint32_t, UniformEntry>> locations{ 1 };
		size_t lock_in()
		{
			locations.emplace_back();
			return locations.size() - 1;
		}
		void clear()
		{
			locations.resize(1);
			locations.back().clear();
		}
		decltype(locations)::const_iterator begin()const { return locations.begin(); }
		decltype(locations)::const_iterator end  ()const { return locations.end  (); }

		//Does not except containers
		//template<typename T, typename = std::enable_if_t<!is_container_v<T>,void>>
		void SetLocation(const mat3& val, uint32_t loc)
		{
			for (auto& v : val)
			{
				SetLocation(v, loc++);
			}
		}
		void SetLocation(const mat4& val, uint32_t loc)
		{
			for (auto& v : val)
			{
				SetLocation(v, loc++);
			}
		}
	};

	struct UboData
	{
		struct UniformEntry
		{
			alignas(sizeof(vec4)) unsigned char data[sizeof(vec4)];
		};
		void SetLocation(const vec4& val, size_t loc)
		{
			if (locations.size() <= loc) locations.resize(loc + 1);
			std::memcpy(locations[loc].data, &val, sizeof(val));
		}
		void SetLocation(const vec3& val, size_t loc)
		{
			if (locations.size() <= loc) locations.resize(loc + 1);
			std::memcpy(locations[loc].data, &val, sizeof(val));
		}
		vector<UniformEntry> locations;

		//Does not except containers
		//template<typename T, typename = std::enable_if_t<!is_container_v<T>,void>>
		void SetLocation(const mat3& val, size_t loc)
		{
			for (auto& v : val)
			{
				SetLocation(v, loc++);
			}
		}
		void SetLocation(const mat4& val, size_t loc)
		{
			for (auto& v : val)
			{
				SetLocation(v, loc++);
			}
		}
	};
	void VulkanPipeline::Create(config_t const& config, Vulkan_t& vulkan)
	{
		auto& m_device = vulkan.Device();
		auto& dispatcher = vulkan.Dispatcher();
		auto& m_renderpass = GetRenderpass(config, vulkan);
		auto [stageCreateInfo, stage_rsc] = GetShaderStageInfo(config, vulkan);

		auto binding_desc = GetVtxBindingInfo(config);
		auto attr_desc = GetVtxAttribInfo(config);

		vk::PipelineVertexInputStateCreateInfo vertexInputInfo{
			vk::PipelineVertexInputStateCreateFlags{}
			,hlp::arr_count(binding_desc)                         //vertexBindingDescriptionCount   
			,std::data(binding_desc)                          //pVertexBindingDescriptions      
			,hlp::arr_count(attr_desc)                            //vertexAttributeDescriptionCount 
			,std::data(attr_desc)                             //pVertexAttributeDescriptions
		};
		auto inputAssembly = GetAssemblyInfo(config);

		auto viewport = GetViewport(config, vulkan);
		auto scissor = GetScissor(config, vulkan);

		vk::PipelineViewportStateCreateInfo viewportState
		{
			vk::PipelineViewportStateCreateFlags{}
			,1,&viewport
			,1,&scissor
		};

		auto rasterizer = GetRasterizerInfo(config);

		auto multisampling = GetMultisampleInfo(config);

		//VkPipelineDepthStencilStateCreateInfo //For depth/stencil buffers; ignored for now

		auto [colorBlending, cb_rsc] = GetColorBlendConfig(config);


		//auto dynamicStates = GetDynamicStates(config);
		//
		//vk::PipelineDynamicStateCreateInfo dynamicState
		//{
		//	vk::PipelineDynamicStateCreateFlags{}
		//	,ArrCount(dynamicStates)            //dynamicStateCount 
		//	,std::data (dynamicStates)//pDynamicStates    
		//};
		//For uniforms
		CreateUniformDescriptors(vulkan, config);//Should probably also allocate the UBO and Descriptors on the View/Data side.
		auto pipelineLayoutInfo = GetLayoutInfo(config);;
		auto m_pipelinelayout = m_device->createPipelineLayoutUnique(pipelineLayoutInfo, nullptr, dispatcher);
		vk::GraphicsPipelineCreateInfo pipelineInfo
		{
			vk::PipelineCreateFlags{}
			,hlp::arr_count(stageCreateInfo),std::data(stageCreateInfo)
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
		pipeline = m_device->createGraphicsPipelineUnique({}, pipelineInfo, nullptr, dispatcher);
	}
	void VulkanPipeline::Reset()
	{
		pipelinelayout.reset();
		pipeline.reset();
	}
	void VulkanPipeline::Bind(const vk::CommandBuffer& cmd_buffer, Vulkan_t& vulkan) const
	{
		cmd_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline, vulkan.Dispatcher());
	}
	bool VulkanPipeline::HasUniforms(const uniform_info& uni) const
	{
		return uni.layouts.size() > 0;
	}
	void VulkanPipeline::BindUniformDescriptions(const vk::CommandBuffer& cmd_buffer, Vulkan_t& vulkan, const uniform_info& uniform)
	{
		if (HasUniforms(uniform))
		{
			//AllocUniformBuffers is meant to write to a host master uniform buffer and get the offset.
			//device master uniform buffer will be updated once all draw calls are queued.
			uint32_t start = AllocUniformBuffers(vulkan, uniform);
			//We can update the buffer and immediately queue a transfer command because we are expecting
			//the command submit order to be (Send Host Uniform Buffer to Device Uniform Buffer) -> (The render pass commands)
			cmd_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *pipelinelayout, start, GetUniformDescriptors(vulkan), nullptr, vulkan.Dispatcher());
		}
	}
	vk::PolygonMode VulkanPipeline::GetPolygonMode(const config_t& config) const
	{
		static const hash_table<FillType, vk::PolygonMode> lookup
		{
			{ eFill,vk::PolygonMode::eFill },
		{ eLine,vk::PolygonMode::eLine },
		};
		return lookup.find(config.fill_type)->second;
	}
	vk::PipelineInputAssemblyStateCreateInfo VulkanPipeline::GetAssemblyInfo(const config_t& config) const
	{
		return vk::PipelineInputAssemblyStateCreateInfo{
			vk::PipelineInputAssemblyStateCreateFlags{}
			,hlp::MapPrimTopology(config.prim_top)
			,config.restart_on_special_idx             //Set to true to allow strips to be restarted with special indices 0xFFFF or 0xFFFFFFFF
		};
	}
	vector<vk::VertexInputAttributeDescription> VulkanPipeline::GetVtxAttribInfo(const config_t& config) const
	{
		[[maybe_unused]] auto [binding, attrib] = hlp::ConvertVtxDesc(config.buffer_descriptions);

		return attrib;
	}
	vector<vk::VertexInputBindingDescription> VulkanPipeline::GetVtxBindingInfo(const config_t& config) const
	{
		[[maybe_unused]] auto [binding, attrib] = hlp::ConvertVtxDesc(config.buffer_descriptions);
		return binding;
	}
	std::pair<vector<vk::PipelineShaderStageCreateInfo>, vector<vk::UniqueShaderModule>> VulkanPipeline::GetShaderStageInfo(const config_t& config, Vulkan_t& vulkan) const
	{
		std::pair<
			vector<vk::PipelineShaderStageCreateInfo>,
			vector<vk::UniqueShaderModule>
		> result;
		auto& [info, rsc] = result;

		auto vert = config.vert_shader;//GetBinaryFile("shaders/vertex.vert.spv");
		auto frag = config.frag_shader;//GetBinaryFile("shaders/fragment.frag.spv");
		rsc.reserve(2);
		auto& fragModule = rsc.emplace_back(vulkan.CreateShaderModule(frag));
		auto& vertModule = rsc.emplace_back(vulkan.CreateShaderModule(vert));

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

	//AllocUniformBuffers is meant to write to a host master(non-vulkan) uniform buffer and get the offset.
	//The non-vulkan buffer will be transfered after the commands are done queueing.


	//Vulkan_t is necessary cause it needs to get the descriptors from the pool

	vk::Viewport VulkanPipeline::GetViewport(const config_t& config, Vulkan_t& vulkan) const
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

	vk::Rect2D VulkanPipeline::GetScissor(const config_t& config, Vulkan_t& vulkan) const
	{
		auto sc = vulkan.Swapchain().extent;
		ivec2 screen_size = (config.screen_size) ? *config.screen_size : ivec2{ s_cast<int>(sc.width),s_cast<int>(sc.height) };
		return vk::Rect2D{
			{ 0,0 },
		{ s_cast<uint32_t>(screen_size.x), s_cast<uint32_t>(screen_size.y) }
		};
	}

	vk::PipelineRasterizationStateCreateInfo VulkanPipeline::GetRasterizerInfo(const config_t& config) const
	{
		return vk::PipelineRasterizationStateCreateInfo
		{
			vk::PipelineRasterizationStateCreateFlags{}
			, VK_FALSE //depthClampEnable VK_FALSE discards fragments that are beyond the depth range, VK_TRUE clamps it instead.
			, VK_FALSE //If rasterizerDiscardEnable is set to VK_TRUE, then geometry never passes through the rasterizer stage. This basically disables any output to the framebuffer.
			, GetPolygonMode(config) //Any other mode requires enabling a GPU feature
			, vk::CullModeFlagBits::eNone
			, vk::FrontFace::eClockwise
			, VK_FALSE
			, 0.0f
			, 0.0f
			, 0.0f
			, 1.0f //Any lines thicker than 1.0f requires wideLines gpu feature.
		};
	}

	vk::PipelineMultisampleStateCreateInfo VulkanPipeline::GetMultisampleInfo(const config_t& config) const
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

	vector<vk::PipelineColorBlendAttachmentState> VulkanPipeline::GetColorBlendAttachments(const config_t& config) const
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

	std::pair<vk::PipelineColorBlendStateCreateInfo, vector<vk::PipelineColorBlendAttachmentState>> VulkanPipeline::GetColorBlendConfig(const config_t& config) const
	{
		//Per frame buffer
		auto colorBlendAttachments = GetColorBlendAttachments(config);
		return std::make_pair(vk::PipelineColorBlendStateCreateInfo
			{
				vk::PipelineColorBlendStateCreateFlags{}
				,VK_FALSE                           //logicOpEnable   
			,vk::LogicOp::eCopy	                //logicOp         
			,hlp::arr_count(colorBlendAttachments)    //attachmentCount 
			,std::data(colorBlendAttachments)     //pAttachments   
			,{ 0.0,0.0f,0.0f,0.0f }
			}, std::move(colorBlendAttachments));
	}

	vector<vk::DynamicState> VulkanPipeline::GetDynamicStates(const config_t& config) const
	{
		return{
			vk::DynamicState::eViewport,
			vk::DynamicState::eLineWidth
		};
	}

	vk::PipelineLayoutCreateInfo VulkanPipeline::GetLayoutInfo(const config_t& config) const
	{
		return vk::PipelineLayoutCreateInfo
		{
			vk::PipelineLayoutCreateFlags{}
			, (uniform_layout)?1u:0u		  //setLayoutCount         
			, (uniform_layout)?&*uniform_layout:nullptr //pSetLayouts            
			, 0		  //pushConstantRangeCount 
			, nullptr //pPushConstantRanges    
		};
	}
	vector<vk::DescriptorSetLayoutBinding> GetDescriptorBindings(const pipeline_config& config)
	{
		auto& ulayout_config = config.uniform_layout;
		vector<vk::DescriptorSetLayoutBinding> result;
		for (auto& binding : ulayout_config.bindings)
		{
			vk::DescriptorSetLayoutBinding a{ 
				binding.binding,
				vk::DescriptorType::eUniformBuffer,
				binding.descriptor_count,
				hlp::MapStages(binding.stages)
				,nullptr
			};

		}
		return result;
	}

	void VulkanPipeline::CreateUniformDescriptors(Vulkan_t& vulkan, const config_t& config)
	{
		auto bindings = GetDescriptorBindings(config);
		if (bindings.size())
		{
			vk::DescriptorSetLayoutCreateInfo layout_info
			{
				vk::DescriptorSetLayoutCreateFlags{}
				,hlp::arr_count(bindings)
				,hlp::arr_count(bindings) ? std::data(bindings) : nullptr
			};
			uniform_layout = vulkan.Device()->createDescriptorSetLayoutUnique(layout_info, nullptr, vulkan.Dispatcher());
		}
	}

	vk::UniqueRenderPass& VulkanPipeline::GetRenderpass(const config_t& config, VulkanView& vulkan)
	{
		return vulkan.Renderpass();
	}

	vector<vk::DescriptorSet> VulkanPipeline::GetUniformDescriptors(Vulkan_t& vulkan)
	{
		return {};
	}

	uint32_t VulkanPipeline::AllocUniformBuffers(Vulkan_t& vulkan, const uniform_info& uniform) { return 0; }
}
