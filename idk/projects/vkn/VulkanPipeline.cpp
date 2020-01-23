#include "pch.h"
#include "VulkanPipeline.h"
#include <vkn/GfxConverters.h>
#include <vkn/BufferHelpers.h>
#include <vkn/ShaderModule.h>
#include <res/ResourceHandle.inl>
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
//#pragma optimize("",off)
	void GetLayouts(const RscHandle<ShaderProgram>& hshader, hash_table<uint32_t, vk::DescriptorSetLayout>& out)
	{
		if (hshader)
		{

			auto& shader = hshader.as<ShaderModule>();
			auto itr = shader.LayoutsBegin();
			for (; itr != shader.LayoutsEnd(); ++itr)
			{
				auto& [set, layout] = *itr;
				out.emplace(set, *layout);
			}

		}
	}

	vk::Pipeline VulkanPipeline::Pipeline() const { 
		return *pipeline;
	}

	vk::PipelineLayout VulkanPipeline::PipelineLayout() const
	{
		return *pipelinelayout;
	}

	void VulkanPipeline::SetRenderPass(vk::RenderPass rp, bool has_depth_stencil)
	{
		_render_pass = rp;
		_has_depth_stencil = has_depth_stencil;
	}

	void VulkanPipeline::ClearRenderPass()
	{
		_render_pass.reset();
		_has_depth_stencil = false;
	}

	std::optional<uint32_t> VulkanPipeline::GetBinding(uint32_t location) const
	{
		std::optional<uint32_t> result;
		auto itr = loc2bind.find(location);
		if (itr != loc2bind.end())
			result = itr->second;
		return result;
	}

	void VulkanPipeline::Create(config_t const& config, vector<vk::PipelineShaderStageCreateInfo> info, Vulkan_t& vulkan, const Options& options)
	{
		hash_table<uint32_t, vk::DescriptorSetLayout> layouts;
		GetLayouts(config.frag_shader, layouts);
		GetLayouts(config.vert_shader, layouts);
		owned_uniform_layouts = FillEmptyLayouts(layouts, vulkan);
		Create(config, info, std::move(layouts), vulkan,options);
	}
	void VulkanPipeline::Create(config_t const& config, vector<RscHandle<ShaderProgram>> shaders, Vulkan_t& vulkan, const Options& options)
	{
		config_t config2 = config;
		hash_table<uint32_t, vk::DescriptorSetLayout> layouts;
		for (auto& shader : shaders)
			GetLayouts(shader, layouts);
		owned_uniform_layouts = FillEmptyLayouts(layouts, vulkan);


		const char* entryPoint = "main";
		vector<vk::PipelineShaderStageCreateInfo> info;
		for (auto& hshader : shaders)
		{
			auto shader = &hshader.as<ShaderModule>();
			auto stage = shader->Stage();
			auto module = shader->Module();
			if (stage == vk::ShaderStageFlagBits::eVertex)
				config2.vert_shader = hshader;
			if (stage == vk::ShaderStageFlagBits::eFragment)
				config2.frag_shader = hshader;
			info.emplace_back(vk::PipelineShaderStageCreateInfo
				{
				vk::PipelineShaderStageCreateFlags{},
				stage ,
				module,
				entryPoint,
				nullptr
				});

		}
		Create(config2, info, std::move(layouts), vulkan, options);
	}
	void VulkanPipeline::Create(const config_t& config, vector<vk::PipelineShaderStageCreateInfo> info, hash_table<uint32_t, vk::DescriptorSetLayout> slayout, Vulkan_t& vulkan, const Options& options)
	{
		auto& m_device = vulkan.Device();
		auto& dispatcher = vulkan.Dispatcher();
		auto m_renderpass = (_render_pass) ? *_render_pass : GetRenderpass(config, vulkan);
		_has_depth_stencil = (_render_pass) ? _has_depth_stencil : (config.render_pass_type != BasicRenderPasses::eRgbaColorOnly);

		auto binding_desc = GetVtxBindingInfo(config);
		auto attr_desc = GetVtxAttribInfo(config);

		loc2bind.clear();

		for (auto& attr : attr_desc)
		{
			loc2bind.emplace(attr.location, attr.binding);
		}

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


		auto dynamicStates = GetDynamicStates(config, options);

		vk::PipelineDynamicStateCreateInfo dynamicState
		{
			vk::PipelineDynamicStateCreateFlags{}
			,hlp::arr_count(dynamicStates)            //dynamicStateCount 
			,std::data(dynamicStates)//pDynamicStates    
		};
		//For uniforms

		{
			uniform_layouts = std::move(slayout);
		}
		auto&& [pipelineLayoutInfo, pli_rsc] = GetLayoutInfo(config);;

		vk::PipelineDepthStencilStateCreateInfo dsci
		{
			vk::PipelineDepthStencilStateCreateFlags{},
			config.depth_test,config.depth_write,vk::CompareOp::eLess,
			VK_FALSE,config.stencil_test,
		};
		auto m_pipelinelayout = m_device->createPipelineLayoutUnique(pipelineLayoutInfo, nullptr, dispatcher);
		vk::PipelineCreateFlags cr8_flags{};
		vk::Pipeline base_pipeline{};
		if (options.is_base_pipeline)
			cr8_flags |= vk::PipelineCreateFlagBits::eAllowDerivatives;

		if (options.derive_from)
		{
			cr8_flags |= vk::PipelineCreateFlagBits::eDerivative;
			base_pipeline = options.derive_from->base;
		}

		vk::GraphicsPipelineCreateInfo pipelineInfo
		{
			cr8_flags
			,hlp::arr_count(info),std::data(info)
			,&vertexInputInfo
			,&inputAssembly
			,nullptr
			,&viewportState
			,&rasterizer
			,&multisampling
			,(_has_depth_stencil) ? &dsci : nullptr
			,std::data(colorBlending)
			,&dynamicState
			,*m_pipelinelayout
			,m_renderpass
			,0
			,base_pipeline
			,0
		};
		pipeline = m_device->createGraphicsPipelineUnique({}, pipelineInfo, nullptr, dispatcher);
		pipelinelayout = std::move(m_pipelinelayout);
	}
	void VulkanPipeline::Create(config_t const& config, vector<std::pair<vk::ShaderStageFlagBits, vk::ShaderModule>> shader_modules, Vulkan_t& vulkan, const Options& options)
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
		Create(config, stageCreateInfo, vulkan,options);
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
	std::pair<vector<vk::PipelineShaderStageCreateInfo>, vector<vk::UniqueShaderModule>> VulkanPipeline::GetShaderStageInfo(const config_t& config, Vulkan_t&) const
	{
		std::pair<
			vector<vk::PipelineShaderStageCreateInfo>,
			vector<vk::UniqueShaderModule>
		> result;
		auto& [info, rsc] = result;

		auto vertModule = [](auto& config) ->std::optional<vk::ShaderModule> 
		{
			if (config.vert_shader)
			{
				ShaderModule& mod = config.vert_shader.as<ShaderModule>();
				if (!mod.HasCurrent())
					mod.UpdateCurrent(0);
				return mod.Module();
			}
			else
				return std::nullopt;
		}(config);
		auto fragModule = [](auto& config) ->std::optional<vk::ShaderModule>
		{
			if (config.frag_shader)
			{
				ShaderModule& mod = config.frag_shader.as<ShaderModule>();
				if (!mod.HasCurrent())
					mod.UpdateCurrent(0);
				return mod.Module();
			}
			else
				return std::nullopt;
		}(config);//rsc.emplace_back(vulkan.CreateShaderModule(frag));

		const char* entryPoint = "main";
		if (fragModule)
		{
			vk::PipelineShaderStageCreateInfo fragShaderStageInfo
			{
				vk::PipelineShaderStageCreateFlags{},
				vk::ShaderStageFlagBits::eFragment,
				*fragModule,
				entryPoint,
				nullptr
			};
			info.emplace_back(fragShaderStageInfo);

		}
		if (vertModule)
		{

		vk::PipelineShaderStageCreateInfo vertShaderStageInfo
		{
			vk::PipelineShaderStageCreateFlags{},
			vk::ShaderStageFlagBits::eVertex,
			*vertModule,
			entryPoint,
			nullptr
		};
		//vk::PipelineShaderStageCreateInfo stageCreateInfo[] = { vertShaderStageInfo,fragShaderStageInfo };
		info.emplace_back(vertShaderStageInfo);
		}
		return result;
	}

	//AllocUniformBuffers is meant to write to a host master(non-vulkan) uniform buffer and get the offset.
	//The non-vulkan buffer will be transfered after the commands are done queueing.


	//Vulkan_t is necessary cause it needs to get the descriptors from the pool

	vk::Viewport VulkanPipeline::GetViewport(const config_t& config, Vulkan_t& vulkan) const
	{
		auto sc = vulkan.Swapchain().extent;
		ivec2 screen_offs = (config.viewport_offset) ? *config.viewport_offset : ivec2{ 0,0 };
		uivec2 screen_size = (config.viewport_size) ? *config.viewport_size : uivec2{ sc.width,sc.height };
		return vk::Viewport
		{
			s_cast<float>(screen_offs.x), s_cast<float>(screen_offs.y), //x,y
			s_cast<float>(screen_size.x), s_cast<float>(screen_size.y),
			0.0f,1.0f // min/max depth
		};
	}

	vk::Rect2D VulkanPipeline::GetScissor(const config_t& config, Vulkan_t& vulkan) const
	{
		auto sc = vulkan.Swapchain().extent;
		auto screen_offs = (config.viewport_offset) ? *config.viewport_offset : ivec2{ 0,0 };
		auto screen_size = (config.viewport_size) ? *config.viewport_size : uivec2{ s_cast<int>(sc.width),s_cast<int>(sc.height) };
		return vk::Rect2D{
			{ screen_offs.x, screen_offs.y },
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
			, static_cast<vk::CullModeFlags>(config.cull_face)
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

	hash_table<BlendFactor, vk::BlendFactor> BlendFactorMap()
	{
		return hash_table<BlendFactor, vk::BlendFactor>
		{
		{BlendFactor::eZero                  ,vk::BlendFactor::eZero                  },// = VK_BLEND_FACTOR_ZERO,
		{BlendFactor::eOne                   ,vk::BlendFactor::eOne                   },// = VK_BLEND_FACTOR_ONE,
		{BlendFactor::eSrcColor              ,vk::BlendFactor::eSrcColor              },// = VK_BLEND_FACTOR_SRC_COLOR,
		{BlendFactor::eOneMinusSrcColor      ,vk::BlendFactor::eOneMinusSrcColor      },// = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,
		{BlendFactor::eDstColor              ,vk::BlendFactor::eDstColor              },// = VK_BLEND_FACTOR_DST_COLOR,
		{BlendFactor::eOneMinusDstColor      ,vk::BlendFactor::eOneMinusDstColor      },// = VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR,
		{BlendFactor::eSrcAlpha              ,vk::BlendFactor::eSrcAlpha              },// = VK_BLEND_FACTOR_SRC_ALPHA,
		{BlendFactor::eOneMinusSrcAlpha      ,vk::BlendFactor::eOneMinusSrcAlpha      },// = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
		{BlendFactor::eDstAlpha              ,vk::BlendFactor::eDstAlpha              },// = VK_BLEND_FACTOR_DST_ALPHA,
		{BlendFactor::eOneMinusDstAlpha      ,vk::BlendFactor::eOneMinusDstAlpha      },// = VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA,
		{BlendFactor::eConstantColor         ,vk::BlendFactor::eConstantColor         },// = VK_BLEND_FACTOR_CONSTANT_COLOR,
		{BlendFactor::eOneMinusConstantColor ,vk::BlendFactor::eOneMinusConstantColor },// = VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR,
		{BlendFactor::eConstantAlpha         ,vk::BlendFactor::eConstantAlpha         },// = VK_BLEND_FACTOR_CONSTANT_ALPHA,
		{BlendFactor::eOneMinusConstantAlpha ,vk::BlendFactor::eOneMinusConstantAlpha },// = VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA,
		{BlendFactor::eSrcAlphaSaturate      ,vk::BlendFactor::eSrcAlphaSaturate      },// = VK_BLEND_FACTOR_SRC_ALPHA_SATURATE,
		{BlendFactor::eSrc1Color             ,vk::BlendFactor::eSrc1Color             },// = VK_BLEND_FACTOR_SRC1_COLOR,
		{BlendFactor::eOneMinusSrc1Color     ,vk::BlendFactor::eOneMinusSrc1Color     },// = VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR,
		{BlendFactor::eSrc1Alpha             ,vk::BlendFactor::eSrc1Alpha             },// = VK_BLEND_FACTOR_SRC1_ALPHA,
		{BlendFactor::eOneMinusSrc1Alpha     ,vk::BlendFactor::eOneMinusSrc1Alpha     },// = VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA
		};
	}

	vk::BlendFactor MapBlendFactor(BlendFactor bf)
	{
		static const auto map = BlendFactorMap();
		return map.find(bf)->second;
	}
	
	hash_table<BlendOp, vk::BlendOp> BlendOpMap()
	{
		return hash_table<BlendOp, vk::BlendOp>
		{
		{BlendOp::eAdd                    ,vk::BlendOp::eAdd                    },// = VK_BLEND_OP_ADD,
		{BlendOp::eSubtract               ,vk::BlendOp::eSubtract               },// = VK_BLEND_OP_SUBTRACT,
		{BlendOp::eReverseSubtract        ,vk::BlendOp::eReverseSubtract        },// = VK_BLEND_OP_REVERSE_SUBTRACT,
		{BlendOp::eMin                    ,vk::BlendOp::eMin                    },// = VK_BLEND_OP_MIN,
		{BlendOp::eMax                    ,vk::BlendOp::eMax                    },// = VK_BLEND_OP_MAX,
		};
	}

	vk::BlendOp MapBlendOp(BlendOp bf)
	{
		static const auto map = BlendOpMap();
		return map.find(bf)->second;
	}

	vk::PipelineColorBlendAttachmentState GetColorBlendAttachment(const AttachmentBlendConfig& config)
	{
		return
			vk::PipelineColorBlendAttachmentState
			{
				/*blendEnable         */config.blend_enable
				/*srcColorBlendFactor */,MapBlendFactor(config.src_color_blend_factor)//optional
				/*dstColorBlendFactor */,MapBlendFactor(config.dst_color_blend_factor)//optional
				/*colorBlendOp        */,MapBlendOp(config.color_blend_op)            //optional
				/*srcAlphaBlendFactor */,MapBlendFactor(config.src_alpha_blend_factor)//optional
				/*dstAlphaBlendFactor */,MapBlendFactor(config.dst_alpha_blend_factor)//optional
				/*alphaBlendOp        */,MapBlendOp(config.alpha_blend_op)            //optional
				/*colorWriteMask      */,vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
			};

	}

	vector<vk::PipelineColorBlendAttachmentState> VulkanPipeline::GetColorBlendAttachments([[maybe_unused]] const config_t& config) const
	{
		//TODO GFX PIPELINE: add colorblending config into pipeline_config_t
		auto& att_config = config.attachment_configs;
		auto size = att_config.size();
		vector<vk::PipelineColorBlendAttachmentState> result(size);
		if(size)
			for (size_t i = 0; i < size; ++i)
			{
				result[i] = GetColorBlendAttachment(att_config[i]);
			}
		else
		{
			result.emplace_back(GetColorBlendAttachment(AttachmentBlendConfig{}));
		}
		return result;

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

	std::pair<vector<vk::PipelineColorBlendStateCreateInfo>, vector<vk::PipelineColorBlendAttachmentState>> VulkanPipeline::GetColorBlendConfig(const config_t& config) const
	{
		//Per frame buffer
		auto colorBlendAttachments = GetColorBlendAttachments(config);
		auto blend_states = vector<vk::PipelineColorBlendStateCreateInfo>{
			vk::PipelineColorBlendStateCreateInfo
			{
				vk::PipelineColorBlendStateCreateFlags{}
				,VK_FALSE                           //logicOpEnable   
				,vk::LogicOp::eCopy	                //logicOp         
				,hlp::arr_count(colorBlendAttachments)    //attachmentCount 
				,std::data(colorBlendAttachments)     //pAttachments   
				,{ 0.0,0.0f,0.0f,0.0f }
			} 
		};
		return std::make_pair(std::move( blend_states), std::move(colorBlendAttachments));
	}

	vector<vk::DynamicState> VulkanPipeline::GetDynamicStates([[maybe_unused]] const config_t& config, const Options& options) const
	{
		//TODO GFX PIPELINE: add dynamic state config into pipeline_config_t
		return options.dynamic_states;
	}
	//TODO GFX PIPELINE: remove the config argument or add push constant configs into config
	std::pair<vk::PipelineLayoutCreateInfo, vector< vk::DescriptorSetLayout>> VulkanPipeline::GetLayoutInfo([[maybe_unused]] const config_t& config) const
	{
		vector<vk::DescriptorSetLayout> layouts;
		uint32_t max = 0;
		for (auto& pair : uniform_layouts) { max = std::max(pair.first+1, max); }
		//max += 1;
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
		return *vulkan.BasicRenderPass(config.render_pass_type);
	}
	//may be deprecated

	vector<vk::DescriptorSet> VulkanPipeline::GetUniformDescriptors([[maybe_unused]]Vulkan_t& vulkan)
	{
		return {};
	}

}
