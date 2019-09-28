#include "pch.h"
#include "VulkanPipeline.h"
#include <vkn/GfxConverters.h>
#include <vkn/BufferHelpers.h>
#include <vkn/ShaderModule.h>
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
	hash_table<uint32_t, vk::UniqueDescriptorSetLayout> FillEmptyLayouts(
		hash_table<uint32_t, vk::DescriptorSetLayout>& layouts
		,VulkanView& view)
	{
		hash_table<uint32_t, vk::UniqueDescriptorSetLayout> result;
		uint32_t max = 0;
		if (layouts.size())
		{
			for ([[maybe_unused]] auto& [set_index, info] : layouts)
			{

				max = std::max(set_index, max);
			}
			++max;
		}
		for (uint32_t i = 0; i < max; ++i)
		{
			vk::DescriptorSetLayoutCreateInfo layout_info
			{
				vk::DescriptorSetLayoutCreateFlags{}
				,0
				,nullptr
			};
			if(layouts.find(i)==layouts.end())
				layouts[i] = *(result[i]=view.Device()->createDescriptorSetLayoutUnique(layout_info, nullptr, view.Dispatcher()));
		}
		return result;
	}
	void GetLayouts(const RscHandle<ShaderProgram>& hshader, hash_table<uint32_t, vk::DescriptorSetLayout>& out)
	{
		auto& frag_shader = hshader.as<ShaderModule>();
		auto itr = frag_shader.LayoutsBegin();
		for (; itr != frag_shader.LayoutsEnd(); ++itr)
		{
			auto& [set, layout] = *itr;
			out.emplace(set, *layout);
		}

	}

	void VulkanPipeline::Create(config_t const& config, vector<vk::PipelineShaderStageCreateInfo> info, Vulkan_t& vulkan)
	{
		auto& m_device = vulkan.Device();
		auto& dispatcher = vulkan.Dispatcher();
		auto m_renderpass = GetRenderpass(config, vulkan);

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

		{
			hash_table<uint32_t, vk::DescriptorSetLayout> layouts;

			GetLayouts(config.frag_shader, layouts);
			GetLayouts(config.vert_shader, layouts);
			owned_uniform_layouts= FillEmptyLayouts(layouts,vulkan);
			uniform_layouts = std::move(layouts);
		}
		auto&& [pipelineLayoutInfo, pli_rsc] = GetLayoutInfo(config);;
		vk::PipelineDepthStencilStateCreateInfo dsci
		{
			vk::PipelineDepthStencilStateCreateFlags{},
			VK_TRUE,VK_TRUE,vk::CompareOp::eLess,
			VK_FALSE,VK_FALSE,
		};
		auto m_pipelinelayout = m_device->createPipelineLayoutUnique(pipelineLayoutInfo, nullptr, dispatcher);
		vk::GraphicsPipelineCreateInfo pipelineInfo
		{
			vk::PipelineCreateFlags{}
			,hlp::arr_count(info),std::data(info)
			,&vertexInputInfo
			,&inputAssembly
			,nullptr
			,&viewportState
			,&rasterizer
			,&multisampling
			,(config.render_pass_type!=BasicRenderPasses::eRgbaColorOnly)?&dsci:nullptr
			,&colorBlending
			,nullptr
			,*m_pipelinelayout
			,m_renderpass
			,0
		};
		pipeline = m_device->createGraphicsPipelineUnique({}, pipelineInfo, nullptr, dispatcher);
		pipelinelayout = std::move(m_pipelinelayout);
	}
	void VulkanPipeline::Create(config_t const& config, vector<std::pair<vk::ShaderStageFlagBits, vk::ShaderModule>> shader_modules, Vulkan_t& vulkan)
	{
		const char* entryPoint = "main";
		vector<vk::PipelineShaderStageCreateInfo> stageCreateInfo;
		for (auto& [stage, module] : shader_modules)
		{
			stageCreateInfo.emplace_back(vk::PipelineShaderStageCreateInfo
				{
				vk::PipelineShaderStageCreateFlags{},
				stage ,
				module,
				entryPoint,
				nullptr
				});

		}		
		Create(config, stageCreateInfo, vulkan);
	}
	void VulkanPipeline::Create(config_t const& config, Vulkan_t& vulkan)
	{
		auto [stageCreateInfo, stage_rsc] = GetShaderStageInfo(config, vulkan);
		Create(config, stageCreateInfo, vulkan);
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
	//Unused for now. If we don't switch to this by Milestone 1, we probably won't need it.
	/*bool VulkanPipeline::HasUniforms(const uniform_info& uni) const
	{
		return uni.layouts.size() > 0;
	}
	//Unused for now. If we don't switch to this by Milestone 1, we probably won't need it.
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
	//Unused for now. If we don't switch to this by Milestone 1, we probably won't need it.
	uint32_t VulkanPipeline::AllocUniformBuffers([[maybe_unused]]Vulkan_t& vulkan, [[maybe_unused]] const uniform_info& uniform) { return 0; }
	*/
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

		auto& vert = config.vert_shader.as<ShaderModule>();//GetBinaryFile("shaders/vertex.vert.spv");
		auto& frag = config.frag_shader.as<ShaderModule>();//GetBinaryFile("shaders/fragment.frag.spv");
		auto fragModule = frag.Module();//rsc.emplace_back(vulkan.CreateShaderModule(frag));
		auto vertModule = vert.Module();//rsc.emplace_back(vulkan.CreateShaderModule(vert));

		const char* entryPoint = "main";

		vk::PipelineShaderStageCreateInfo fragShaderStageInfo
		{
			vk::PipelineShaderStageCreateFlags{},
			vk::ShaderStageFlagBits::eFragment,
			fragModule,
			entryPoint,
			nullptr
		};

		vk::PipelineShaderStageCreateInfo vertShaderStageInfo
		{
			vk::PipelineShaderStageCreateFlags{},
			vk::ShaderStageFlagBits::eVertex,
			vertModule,
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

	vk::PipelineMultisampleStateCreateInfo VulkanPipeline::GetMultisampleInfo([[maybe_unused]] const config_t& config) const
	{
		//TODO GFX PIPELINE: add multisample config into pipeline_config_t
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

	vector<vk::PipelineColorBlendAttachmentState> VulkanPipeline::GetColorBlendAttachments([[maybe_unused]] const config_t& config) const
	{
		//TODO GFX PIPELINE: add colorblending config into pipeline_config_t
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

	vector<vk::DynamicState> VulkanPipeline::GetDynamicStates([[maybe_unused]] const config_t& config) const
	{
		//TODO GFX PIPELINE: add dynamic state config into pipeline_config_t
		return{
			vk::DynamicState::eViewport,
			vk::DynamicState::eLineWidth
		};
	}
	//TODO GFX PIPELINE: remove the config argument or add push constant configs into config
	std::pair<vk::PipelineLayoutCreateInfo, vector< vk::DescriptorSetLayout>> VulkanPipeline::GetLayoutInfo([[maybe_unused]] const config_t& config) const
	{
		vector<vk::DescriptorSetLayout> layouts;
		uint32_t max = 0;
		for (auto& pair : uniform_layouts) { max = std::max(pair.first, max); }
		max += 1;
		layouts.resize(max);
		for (auto& [index, info] : uniform_layouts)
		{
			layouts[index] = info;
		}
		//std::transform(uniform_layouts.begin(), uniform_layouts.end(), std::back_inserter(layouts), [](auto& u) {return *u.second; });
		return make_pair(vk::PipelineLayoutCreateInfo
		{
			vk::PipelineLayoutCreateFlags{}
			, s_cast<uint32_t>(layouts.size())		  //setLayoutCount         
			, (layouts.size()) ? std::data(layouts) : nullptr //pSetLayouts            
			, 0		  //pushConstantRangeCount 
			, nullptr //pPushConstantRanges    
		},std::move(layouts));
	}

	vk::RenderPass VulkanPipeline::GetRenderpass([[maybe_unused]]const config_t& config, [[maybe_unused]] VulkanView& vulkan)
	{

		//TODO GFX PIPELINE:  add renderpass id into pipeline_config_t
		//TODO GFX PIPELINE:  get renderpass using renderpass id in pipeline_config_t
		return vulkan.BasicRenderPass(config.render_pass_type);
	}
	//may be deprecated

	vector<vk::DescriptorSet> VulkanPipeline::GetUniformDescriptors([[maybe_unused]]Vulkan_t& vulkan)
	{
		return {};
	}

}
