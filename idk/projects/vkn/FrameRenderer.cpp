#include "pch.h"
#include "FrameRenderer.h"
#include <gfx/GraphicsSystem.h> //GraphicsState
#include <vkn/VulkanPipeline.h>
#include <vkn/VulkanMesh.h>
#include <file/FileSystem.h>
#include <vkn/ShaderModule.h>
#include <vkn/PipelineManager.h>
#include <math/matrix_transforms.h>
namespace idk::vkn
{
	struct SomeHackyThing
	{
		VulkanPipeline pipeline;
	};
	static SomeHackyThing thing;
	void InitThing(VulkanView& view)
	{

		uint32_t pos_loc = 0;
		uint32_t nml_loc = 0;
		uint32_t pos_binding = 0;
		uint32_t nml_binding = 1;
		//uint32_t zzz_binding = 2;
		pipeline_config config{};
		buffer_desc pos_desc{};
		buffer_desc nml_desc{};

		pos_desc.AddAttribute(AttribFormat::eSVec3, pos_loc, 0);
		nml_desc.AddAttribute(AttribFormat::eSVec3, nml_loc, 0);
		pos_desc.binding.binding_index = pos_binding;
		pos_desc.binding.stride = sizeof(vec3);
		pos_desc.binding.vertex_rate = eVertex;
		nml_desc.binding.binding_index = nml_binding;
		nml_desc.binding.stride = sizeof(vec3);
		nml_desc.binding.vertex_rate = eVertex;

		config.buffer_descriptions.emplace_back(pos_desc);
		config.buffer_descriptions.emplace_back(nml_desc);
		string f, v;
		{
			std::stringstream stringify;
			{
				auto vbuffer = Core::GetSystem<FileSystem>().Open("/assets/shader/mesh.vert.spv", FS_PERMISSIONS::READ, true);
				stringify << vbuffer.rdbuf();
			}
			v = stringify.str();
			config.vert_shader = v;

		}
		{
			std::stringstream stringify;
			{
				auto vbuffer = Core::GetSystem<FileSystem>().Open("/assets/shader/flat_color.frag.spv", FS_PERMISSIONS::READ, true);
				stringify << vbuffer.rdbuf();
			}
			f = stringify.str();
			config.frag_shader = f;
		}
		config.prim_top = PrimitiveTopology::eTriangleList;
		config.fill_type = FillType::eFill;
		//config.uniform_layouts.emplace()
		thing.pipeline.Create(config, view);
	}
	void RenderStateV2::Reset() {
		cmd_buffer.reset({}, vk::DispatchLoaderDefault{});
		ubo_manager.Clear();
		dpools.Reset();
		has_commands = false;
	}
	buffer_desc BufferDesc(uint32_t pos_loc, uint32_t pos_binding, AttribFormat format, uint32_t stride, VertexRate rate)
	{
		buffer_desc pos_desc{};

		pos_desc.AddAttribute(format, pos_loc, 0);
		pos_desc.binding.binding_index = pos_binding;
		pos_desc.binding.stride = stride;
		pos_desc.binding.vertex_rate = rate;

		return pos_desc;
	}
	void FrameRenderer::Init(VulkanView* view, vk::CommandPool cmd_pool) {
		//Todo: Initialize the stuff
		_view = view;
		//Do only the stuff per frame
		//uint32_t num_fo = instance_->View().Swapchain().frame_objects.size();
		uint32_t num_fo = 1;
		uint32_t num_concurrent_states = 1;
		//frames.resize(num_fo);
		_cmd_pool = cmd_pool;
		auto device = *View().Device();
		auto pri_buffers = device.allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo{ cmd_pool,vk::CommandBufferLevel::ePrimary, num_fo }, vk::DispatchLoaderDefault{});
		_pri_buffer = std::move(pri_buffers[0]);
		auto t_buffers = device.allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo{ cmd_pool,vk::CommandBufferLevel::eSecondary, num_fo }, vk::DispatchLoaderDefault{});
		{
			_transition_buffer = std::move(t_buffers[0]);
		}

		GrowStates(num_concurrent_states);
		//Temp
		for (auto i = num_concurrent_states; i-- > 0;)
		{
			auto thread = std::make_unique<NonThreadedRender>();
			thread->Init(this);
			_render_threads.emplace_back(std::move(thread));
		}
		InitThing(View());


		//TODO figure this out
		string filename = "/assets/shader/mesh.vert";
		auto actualfile = Core::GetSystem<FileSystem>().GetFile(filename + "spv");
		auto rsc = Core::GetResourceManager().GetFileResources(actualfile);
		if (!actualfile || !rsc.resources.size())
		{

			vector<buffer_desc> desc{
				BufferDesc(0, 0, AttribFormat::eSVec3, sizeof(vec3), eVertex),
				BufferDesc(0, 1, AttribFormat::eSVec3, sizeof(vec3), eVertex),
				BufferDesc(0, 2, AttribFormat::eSVec2, sizeof(vec2), eVertex),
			};
			auto file = Core::GetSystem<FileSystem>().GetFile(filename);
			;
			//Load the glsl, which will spit out a .___spv
			Core::GetResourceManager().LoadFile(file);
			Core::GetSystem<FileSystem>().Update();
			actualfile = Core::GetSystem<FileSystem>().GetFile(filename+"spv");
			_mesh_renderer_shader_module = Core::GetResourceManager().LoadFile(actualfile).resources.front().As<ShaderProgram>();
			_mesh_renderer_shader_module.as<ShaderModule>().AttribDescriptions(std::move(desc));
			//_mesh_renderer_shader_module.as<ShaderModule>().Load(vk::ShaderStageFlagBits::eVertex,std::move(desc), strm.str());
			//_mesh_renderer_shader_module = Core::GetResourceManager().Create<ShaderModule>();

		}
		else
		{
			_mesh_renderer_shader_module = rsc.resources.front().As<ShaderProgram>();
		}
	}
	void FrameRenderer::SetPipelineManager(PipelineManager& manager)
	{
		_pipeline_manager = &manager;
	}
	void FrameRenderer::RenderGraphicsStates(const vector<GraphicsState>& gfx_states)
	{
		auto& curr_frame = *this;
		GrowStates(gfx_states.size());
		size_t num_concurrent = curr_frame._render_threads.size();
		auto& pri_buffer = curr_frame._pri_buffer;
		auto& transition_buffer = curr_frame._transition_buffer;
		auto queue = View().GraphicsQueue();
		auto& swapchain = View().Swapchain();
		for (auto& state : curr_frame._states)
		{
			state.Reset();//cmd_buffer.reset({}, vk::DispatchLoaderDefault{});
		}
		for (size_t i = 0; i + num_concurrent <= gfx_states.size(); i += num_concurrent)
		{
			//Spawn/Assign to the threads
			for (size_t j = 0; j < num_concurrent; ++j) {
				auto& state = gfx_states[i + j];
				auto& rs = _states[i + j];
				_render_threads[j]->Render(state, rs);
				//RenderGraphicsState(state, curr_frame.states[j]);//We may be able to multi thread this
			}
			//Wait here
			for (auto& thread : _render_threads) {
				thread->Join();
			}
		}
		pri_buffer->reset({}, vk::DispatchLoaderDefault{});
		vk::CommandBufferBeginInfo begin_info{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit };
		vector<vk::CommandBuffer> buffers{};
		for (auto& state : curr_frame._states)
		{
			if (state.has_commands)
				buffers.emplace_back(state.cmd_buffer);
		}
		pri_buffer->begin(begin_info, vk::DispatchLoaderDefault{});
		//if(buffers.size())
		//	pri_buffer->executeCommands(buffers, vk::DispatchLoaderDefault{});
		vk::CommandBufferInheritanceInfo iinfo
		{
		};



		vk::ImageSubresourceRange subResourceRange = {};
		subResourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		subResourceRange.baseMipLevel = 0;
		subResourceRange.levelCount = 1;
		subResourceRange.baseArrayLayer = 0;
		subResourceRange.layerCount = 1;

		vk::ImageMemoryBarrier presentToClearBarrier = {};
		presentToClearBarrier.srcAccessMask = vk::AccessFlags{};
		presentToClearBarrier.dstAccessMask = vk::AccessFlags{};
		presentToClearBarrier.oldLayout = vk::ImageLayout::eUndefined;
		presentToClearBarrier.newLayout = vk::ImageLayout::ePresentSrcKHR;
		presentToClearBarrier.srcQueueFamilyIndex = *View().QueueFamily().graphics_family;
		presentToClearBarrier.dstQueueFamilyIndex = *View().QueueFamily().graphics_family;
		presentToClearBarrier.image = swapchain.images[swapchain.curr_index];
		presentToClearBarrier.subresourceRange = subResourceRange;
		begin_info.pInheritanceInfo = &iinfo;
		transition_buffer->begin(begin_info, vk::DispatchLoaderDefault{});
		transition_buffer->pipelineBarrier(vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eAllCommands, vk::DependencyFlags{}, nullptr, nullptr, presentToClearBarrier, vk::DispatchLoaderDefault{});
		transition_buffer->end();
		//hlp::TransitionImageLayout(*transition_buffer, queue, swapchain.images[swapchain.curr_index], vk::Format::eUndefined, vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR,&iinfo);
		pri_buffer->executeCommands(*transition_buffer, vk::DispatchLoaderDefault{});
		pri_buffer->end();

		auto& current_signal = View().CurrPresentationSignals();

		auto& waitSemaphores = *current_signal.image_available;
		auto& readySemaphores = *_states[0].signal.render_finished;
		vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eAllCommands };

		//std::vector<vk::CommandBuffer> cmd_buffers;
		//for (auto& state : curr_frame.states)
		//{
		//	cmd_buffers.emplace_back(state.cmd_buffer);
		//}
		//cmd_buffers.emplace_back(*pri_buffer);
		vk::SubmitInfo submit_info
		{
			1
			,&waitSemaphores
			,waitStages
			,hlp::arr_count(buffers),std::data(buffers)
			,1,&readySemaphores
		};


		View().Device()->resetFences(1, &*_states[0].signal.inflight_fence, vk::DispatchLoaderDefault{});
		queue.submit(submit_info, *_states[0].signal.inflight_fence, vk::DispatchLoaderDefault{});
	}
	PresentationSignals& FrameRenderer::GetMainSignal()
	{
		return _states[0].signal;
	}
	void FrameRenderer::GrowStates(size_t new_min_size)
	{
		auto cmd_pool = *View().Commandpool();
		auto device = *View().Device();
		if (new_min_size > _states.size())
		{
			auto diff = s_cast<uint32_t>(new_min_size - _states.size());
			auto&& buffers = device.allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo{ cmd_pool,vk::CommandBufferLevel::ePrimary, diff }, vk::DispatchLoaderDefault{});
			for (auto i = diff; i-- > 0;)
			{
				auto& buffer = buffers[i];
				_states.emplace_back(RenderStateV2{ *buffer,UboManager{View()},PresentationSignals{},DescriptorsManager{View()} }).signal.Init(View());
				_state_cmd_buffers.emplace_back(std::move(buffer));
			}
		}
	}
	RscHandle<ShaderProgram> FrameRenderer::GetMeshRendererShaderModule()
	{
		return _mesh_renderer_shader_module;
	}

	string GetUniformData(const UniformInstance& uniform)
	{
		string data;
		return data;
	}

	std::pair<vector<FrameRenderer::ProcessedRO>, FrameRenderer::DsBindingCount> FrameRenderer::ProcessRoUniforms(const GraphicsState& state, UboManager& ubo_manager)
	{
		const vector<RenderObject>& draw_calls = state.mesh_render;
		const Camera& cam = state.camera;
		std::pair<vector<ProcessedRO>, DsBindingCount> result{};
		DsBindingCount& collated_layouts = result.second;

		auto msprog = GetMeshRendererShaderModule();
		auto& msmod = msprog.as<ShaderModule>();
		auto& obj_uni = msmod.GetLayout("MVP");
		auto& nml_uni = msmod.GetLayout("MVP_IVT");
		auto VP = translate(vec3{ 0,0,0.0f });//cam.ProjectionMatrix() * cam.ViewMatrix();
		;
		//Force pipeline creation
		vector<RscHandle<ShaderProgram>> shaders;
		for (auto& dc : draw_calls)
		{
			//Force pipeline creation
			shaders.resize(0);
			shaders.emplace_back(GetMeshRendererShaderModule());
			auto sprog = dc.material_instance.material->GetShaderProgram();
			shaders.emplace_back(sprog);
			//TODO Grab everything and render them
			//Maybe change the config to be a managed resource.
			//Force pipeline creation
			GetPipeline(state.config, shaders);
			//set, bindings
			hash_table < uint32_t, vector<ProcessedRO::BindingInfo>> collated_bindings;
			auto& layouts = sprog.as<ShaderModule>();
			//Account for the object and normal transform bindings
			collated_layouts[obj_uni.layout]++;
			collated_layouts[nml_uni.layout]++;
			mat4 obj_trf = VP* dc.transform;
			mat4 obj_ivt= obj_trf.inverse().transpose();
			auto&& [trf_buffer, trf_offset] = ubo_manager.Add(obj_trf);
			auto&& [ivt_buffer, ivt_offset] = ubo_manager.Add(obj_ivt);


			collated_bindings[obj_uni.set].emplace_back(
				ProcessedRO::BindingInfo
				{
					obj_uni.binding,
					trf_buffer,
					trf_offset,
					0,
					obj_uni.size
				}
			);
			collated_bindings[nml_uni.set].emplace_back(
				ProcessedRO::BindingInfo
				{
					nml_uni.binding,
					ivt_buffer,
					ivt_offset,
					0,
					nml_uni.size
				}
			);

			//Account for material bindings
			for (auto& pair: dc.material_instance.uniforms)
			{
				auto& name = pair.first;
				auto& itr = layouts.GetLayout(name);
				auto& layout = itr.layout;
				{
					collated_layouts[layout]++;
					auto&& data = dc.material_instance.GetUniformBlock(name);
					auto&& [buffer, offset] = ubo_manager.Add(data);
					collated_bindings[itr.set].emplace_back(
						ProcessedRO::BindingInfo
						{
							itr.binding,
							buffer,
							offset,
							0,
							itr.size
						}
					);
				}
			}
			result.first.emplace_back(ProcessedRO{ &dc,std::move(collated_bindings) });
		}
		return result;
	}

	void UpdateUniformDS(
		vk::Device& device,
		vk::DescriptorSet& dset,
		vector<ProcessedRO::BindingInfo> bindings
	)
	{
		//TODO: Handle Other DSes as well
		for (auto& binding : bindings)
		{
			//auto& dset = ds2[i++];
			vk::DescriptorBufferInfo bufferInfo[]
			{
				vk::DescriptorBufferInfo{
				binding.ubuffer
				, binding.buffer_offset
				, binding.size
				}
			};
			;

			vector<vk::WriteDescriptorSet> descriptorWrite
			{
				vk::WriteDescriptorSet{
					dset
					,binding.binding
					,binding.arr_index
					,hlp::arr_count(bufferInfo)
					,vk::DescriptorType::eUniformBuffer
					,nullptr
					,std::data(bufferInfo)
					,nullptr
				}
			};
			device.updateDescriptorSets(descriptorWrite, nullptr, vk::DispatchLoaderDefault{});
		}
	}

	void FrameRenderer::RenderGraphicsState(const GraphicsState& state, RenderStateV2& rs)
	{
		auto& swapchain = View().Swapchain();
		auto dispatcher = vk::DispatchLoaderDefault{};
		vk::CommandBuffer& cmd_buffer = rs.cmd_buffer;
		vk::CommandBufferInheritanceInfo aaa{};
		vk::CommandBufferBeginInfo begin_info{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit,&aaa };
		cmd_buffer.begin(begin_info, dispatcher);
		std::array<float, 4> a{};
		//TODO grab the appropriate framebuffer and begin renderpass
		vk::ClearValue v{ vk::ClearColorValue{ r_cast<const std::array<float,4>&>(state.camera.clear_color) } };
		auto sz = View().GetWindowsInfo().size;
		vk::Rect2D render_area
		{
			vk::Offset2D{},vk::Extent2D
			{
				s_cast<uint32_t>(sz.x),s_cast<uint32_t>(sz.y)
			}
		};
		vk::RenderPassBeginInfo rpbi
		{
			*View().Renderpass(), *swapchain.frame_buffers[swapchain.curr_index],
			render_area,1,&v
		};


		cmd_buffer.beginRenderPass(rpbi, vk::SubpassContents::eInline, dispatcher);

		VulkanPipeline* prev_pipeline=nullptr;
		vector<RscHandle<ShaderProgram>> shaders;

		//Preprocess MeshRender's uniforms
		auto&& [processed_ro, layout_count] = ProcessRoUniforms(state, rs.ubo_manager);
		rs.ubo_manager.UpdateAllBuffers();
		auto alloced_dsets = rs.dpools.Allocate(layout_count);

		for (auto& p_ro : processed_ro)
		{
			auto& obj = p_ro.Object();
			rs.FlagRendered();
			shaders.resize(0);
			shaders.emplace_back(GetMeshRendererShaderModule());
			auto msprog = GetMeshRendererShaderModule();
			auto sprog = obj.material_instance.material->GetShaderProgram();
			shaders.emplace_back(sprog);
			//TODO Grab everything and render them
			//Maybe change the config to be a managed resource.
			auto& pipeline = GetPipeline(state.config,shaders);
			//auto& mat = obj.material_instance.material.as<VulkanMaterial>();
			if (&pipeline != prev_pipeline)
			{
				pipeline.Bind(cmd_buffer, View());
				prev_pipeline = &pipeline;
			}
			auto& mesh = obj.mesh.as<VulkanMesh>();
			auto& layouts = pipeline.uniform_layouts;
			for (auto& [set_index,binfo] : p_ro.bindings)
			{
				//Get the descriptor set layout for the current set
				auto layout_itr = layouts.find(set_index);
				if (layout_itr != layouts.end())
				{
					//Find the allocated pool of descriptor sets that matches the descriptor set layout
					auto ds_itr = alloced_dsets.find(*layout_itr->second);
					if (ds_itr == alloced_dsets.end())
						ds_itr = alloced_dsets.find(*layout_itr->second);
					if(ds_itr!=alloced_dsets.end())
					{
						//Get a descriptor set from the allocated pool
						auto ds = ds_itr->second.GetNext();
						//Update the descriptor set
						UpdateUniformDS(*View().Device(),ds,binfo);
						cmd_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *pipeline.pipelinelayout, set_index, ds, nullptr, dispatcher);
					}
				}
			}
			
			auto& bindings = obj.attrib_bindings;
			for (auto&& [bindingz, attrib] : bindings)
			{
				auto& attrib_buffer = mesh.Get(attrib);
				cmd_buffer.bindVertexBuffers(bindingz, *attrib_buffer.buffer, vk::DeviceSize{ 0 }, vk::DispatchLoaderDefault{});
			}
			auto& oidx = mesh.GetIndexBuffer();
			if (oidx)
			{
				cmd_buffer.bindIndexBuffer(*(*oidx).buffer, 0, mesh.IndexType(), vk::DispatchLoaderDefault{});
				cmd_buffer.drawIndexed(mesh.IndexCount(), 1, 0, 0, 0, vk::DispatchLoaderDefault{});
			}
		}
		cmd_buffer.endRenderPass();
		cmd_buffer.end();
	}

	PipelineManager& FrameRenderer::GetPipelineManager()
	{
		return *_pipeline_manager;
	}

	VulkanPipeline& FrameRenderer::GetPipeline(const pipeline_config& config,const vector<RscHandle<ShaderProgram>>& modules)
	{
		// TODO: Replace with something that actually gets the pipeline
		return GetPipelineManager().GetPipeline(config,modules);
	}

	//PipelineHandle_t FrameRenderer::GetPipelineHandle()
	//{
	//	// TODO: Replace with something that actually figures out what pipeline to use
	//	return PipelineHandle_t{};
	//}

	void FrameRenderer::NonThreadedRender::Init(FrameRenderer* renderer)
	{
		_renderer = renderer;
	}

	void FrameRenderer::NonThreadedRender::Render(const GraphicsState& state, RenderStateV2& rs)
	{
		_renderer->RenderGraphicsState(state, rs);
	}

	void FrameRenderer::NonThreadedRender::Join()
	{
	}

}