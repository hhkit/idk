#include "pch.h"
#include <forward_list>
#include "FrameRenderer.h"
#include <core/Core.h>
#include <gfx/GraphicsSystem.h> //GraphicsState
#include <vkn/VulkanPipeline.h>
#include <vkn/VulkanMesh.h>
#include <file/FileSystem.h>
#include <vkn/ShaderModule.h>
#include <vkn/PipelineManager.h>
#include <math/matrix_transforms.h>
#include <vkn/GraphicsState.h>
#include <gfx/RenderTarget.h>
#include <vkn/VknFrameBuffer.h>
#include <vkn/VknRenderTarget.h>
#include <gfx/Light.h>

#include <gfx/MeshRenderer.h>
#include <anim/SkinnedMeshRenderer.h>
#include <vkn/vulkan_enum_info.h>
#include <vkn/VknTexture.h>
#include <vkn/VulkanHashes.h>

#include <vkn/PipelineBinders.inl>
#include <vkn/VknCubemap.h>

#include <gfx/ViewportUtil.h>
#include <vkn/VknCubeMapLoader.h>
#include <vkn/VulkanCbmLoader.h>

#include <cstdint>

namespace idk::vkn
{
	using collated_bindings_t = hash_table < uint32_t, vector<ProcessedRO::BindingInfo>>;//Set, bindings

	void SetViewport(vk::CommandBuffer cmd_buffer, ivec2 vp_pos, ivec2 vp_size)
	{
		vk::Viewport vp{ s_cast<float>(vp_pos.x),s_cast<float>(vp_pos.y),s_cast<float>(vp_size.x),s_cast<float>(vp_size.y),0,1 };
		cmd_buffer.setViewport(0, vp);
	}
	std::pair<ivec2, ivec2> ComputeVulkanViewport(const vec2& sz, const Viewport& vp)
	{
		auto pair = ComputeViewportExtents(sz, vp);
		auto& [offset, size] = pair;

		offset = ivec2{ (translate(ivec2{ 0,sz.y }) * tmat<int,3,3> { scale(ivec2{ 1,-1 }) }).transpose()* ivec3 { offset,1 } };//  -ivec2{ 0,size.y };
		return pair;
	}

	template<typename T>
	ProcessedRO::BindingInfo CreateBindingInfo(const UboInfo& obj_uni, const T& val, FrameRenderer::DsBindingCount& collated_layouts, UboManager& ubo_manager)
	{
		collated_layouts[obj_uni.layout].first = vk::DescriptorType::eUniformBuffer;
		//collated_layouts[obj_uni.layout].second++;
		auto&& [trf_buffer, trf_offset] = ubo_manager.Add(val);
		//collated_bindings[obj_uni.set].emplace_back(
		return ProcessedRO::BindingInfo
		{
			obj_uni.binding,
			trf_buffer,
			trf_offset,
			0,
			obj_uni.size,
			obj_uni.layout
		};
		//);
	}

	template<typename T>
	void PreProcUniform(const UboInfo& obj_uni, const T& val, FrameRenderer::DsBindingCount& collated_layouts, collated_bindings_t& collated_bindings, UboManager& ubo_manager)
	{
		//collated_layouts[obj_uni.layout].first = vk::DescriptorType::eUniformBuffer;
		////collated_layouts[obj_uni.layout].second++;
		//auto&& [trf_buffer, trf_offset] = ubo_manager.Add(val);
		auto& bindings = collated_bindings[obj_uni.set];
		bindings.emplace_back(
			CreateBindingInfo(obj_uni, val, collated_layouts, ubo_manager)
			//ProcessedRO::BindingInfo
			//{
			//	obj_uni.binding,
			//	trf_buffer,
			//	trf_offset,
			//	0,
			//	obj_uni.size
			//}
		);
	}
	template<typename WTF>
	void PreProcUniform(const UboInfo& obj_uni, uint32_t index, RscHandle<Texture> val, FrameRenderer::DsBindingCount& collated_layouts, collated_bindings_t& collated_bindings)
	{
		collated_layouts[obj_uni.layout].first = vk::DescriptorType::eCombinedImageSampler;
		//collated_layouts[obj_uni.layout].second++;
		//auto&& [trf_buffer, trf_offset] = ubo_manager.Add(val);
		auto& texture = val.as<VknTexture>();
		auto& bindings = collated_bindings[obj_uni.set];
		bindings.emplace_back(
			ProcessedRO::BindingInfo
			{
				obj_uni.binding,
				ProcessedRO::image_t{texture.ImageView(),*texture.sampler,vk::ImageLayout::eGeneral},
				0,
				index,
				obj_uni.size,
				obj_uni.layout
			}
		);
	}
	template<typename WTF>
	void PreProcUniform(const UboInfo& obj_uni,uint32_t index, RscHandle<RenderTarget> val, FrameRenderer::DsBindingCount& collated_layouts, collated_bindings_t& collated_bindings)
	{
		//collated_layouts[obj_uni.layout].first = vk::DescriptorType::eCombinedImageSampler;
		//collated_layouts[obj_uni.layout].second++;
		//auto&& [trf_buffer, trf_offset] = ubo_manager.Add(val);
		PreProcUniform<WTF>(obj_uni,index, val->GetDepthBuffer(),collated_layouts, collated_bindings);
	}
	template<typename lol=void>
	void BindBones(const UboInfo& info,const AnimatedRenderObject& aro, const vector<SkeletonTransforms>& bones, UboManager& ubos, FrameRenderer::DsBindingCount & collated_layouts, collated_bindings_t & collated_bindings)
	{
		auto&&[buffer,offset]=ubos.Add(bones[aro.skeleton_index]);
		auto& bindings = collated_bindings[info.set];
		bindings.emplace_back(info.binding, buffer, offset, 0, info.size);
	}
	struct GraphicsStateInterface
	{
		//RscHandle<ShaderProgram>             mesh_vtx;
		//RscHandle<ShaderProgram>             skinned_mesh_vtx;
		array<RscHandle<ShaderProgram>, VertexShaders::VMax>   renderer_vertex_shaders;
		array<RscHandle<ShaderProgram>, FragmentShaders::FMax>   renderer_fragment_shaders;
		const vector<const RenderObject*>*         mesh_render;
		const vector<const AnimatedRenderObject*>* skinned_mesh_render;
		GraphicsStateInterface() = default;
		GraphicsStateInterface(const GraphicsState& state)
		{
			renderer_vertex_shaders = state.renderer_vertex_shaders;
			renderer_fragment_shaders = state.renderer_fragment_shaders;
			mesh_render = &state.mesh_render;
			skinned_mesh_render = &state.skinned_mesh_render;
		}
		GraphicsStateInterface(const PreRenderData& state)
		{
			renderer_vertex_shaders = state.renderer_vertex_shaders;
			renderer_fragment_shaders = state.renderer_fragment_shaders;
			mesh_render = &state.mesh_render;
			skinned_mesh_render = &state.skinned_mesh_render;
		}
	};


	PipelineThingy ProcessRoUniforms(const GraphicsStateInterface& state, UboManager& ubo_manager,StandardBindings& binders)
	{
		auto& mesh_vtx            = state.renderer_vertex_shaders[VNormalMesh];
		auto& mesh_render         = *state.mesh_render;
		auto& skinned_mesh_vtx    = state.renderer_vertex_shaders[VSkinnedMesh];
		auto& skinned_mesh_render = *state.skinned_mesh_render;

		//auto& binders = *binder;
		PipelineThingy the_interface{};
		the_interface.SetRef(ubo_manager);

		the_interface.BindShader(ShaderStage::Vertex, mesh_vtx);
		binders.Bind(the_interface);
		{
			const vector<const RenderObject*>& draw_calls = mesh_render;
			for (auto& ptr_dc : draw_calls)
			{
				auto& dc = *ptr_dc;
				auto& mat_inst = *dc.material_instance;
				if (mat_inst.material)
				{
					binders.Bind(the_interface, dc);

					the_interface.FinalizeDrawCall(dc);
				}
			}//End of draw_call loop
		}

		{
			const vector<const AnimatedRenderObject*>& draw_calls = skinned_mesh_render;
			the_interface.BindShader(ShaderStage::Vertex, skinned_mesh_vtx);
			binders.Bind(the_interface);
			for (auto& ptr_dc : draw_calls)
			{
				auto& dc = *ptr_dc;
				auto& mat_inst = *dc.material_instance;
				if (mat_inst.material)
				{
					binders.Bind(the_interface, dc);

					the_interface.FinalizeDrawCall(dc);

				}
			}//End of draw_call loop
		}


		return std::move(the_interface);
	}
	PipelineThingy ProcessRoUniforms(const GraphicsState& state, UboManager& ubo_manager, StandardBindings& binders)
	{
		return ProcessRoUniforms(GraphicsStateInterface{ state }, ubo_manager, binders);
	}
	template<typename T,typename...Args>
	using has_setstate = decltype(std::declval<T>().SetState(std::declval<Args>()...));
	//Possible Idea: Create a Pipeline object that tracks currently bound descriptor sets
	PipelineThingy FrameRenderer::ProcessRoUniforms(const GraphicsState& state, UboManager& ubo_manager)
	{
		if (state.camera.is_shadow)
		{
			ShadowBinding binders;
			binders.for_each_binder([](auto& binder, const GraphicsState& state) {binder.SetState(state); }, state);
			return vkn::ProcessRoUniforms(state, ubo_manager, binders);
		}
		else
		{
			PbrFwdMaterialBinding binders;
			binders.for_each_binder<has_setstate>([](auto& binder, const GraphicsState& state) {binder.SetState(state); }, state);
			return vkn::ProcessRoUniforms(state,ubo_manager,binders);

		}

	}

	vk::Framebuffer GetFrameBuffer(const CameraData& camera_data, uint32_t)
	{
		//TODO Actually get the framebuffer from camera_data
		//auto& e = camera_data.render_target.as<VknFrameBuffer>();
		return camera_data.render_target.as<VknRenderTarget>().Buffer();
	}



	RscHandle<ShaderProgram> LoadShader(string filename)
	{
		return *Core::GetResourceManager().Load<ShaderProgram>(filename, false);
	}
	void FrameRenderer::Init(VulkanView* view, vk::CommandPool cmd_pool) {
		//Todo: Initialize the stuff
		_view = view;
		//Do only the stuff per frame
		uint32_t num_fo = 1;
		uint32_t num_concurrent_states = 1;

		_cmd_pool = cmd_pool;
		auto device = *View().Device();
		auto pri_buffers = device.allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo{ cmd_pool,vk::CommandBufferLevel::ePrimary, num_fo }, vk::DispatchLoaderDefault{});
		_pri_buffer = std::move(pri_buffers[0]);
		auto t_buffers = device.allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo{ cmd_pool,vk::CommandBufferLevel::eSecondary, num_fo }, vk::DispatchLoaderDefault{});
		{
			_transition_buffer = std::move(t_buffers[0]);
		}

		GrowStates(_states,num_concurrent_states);
		//Temp
		for (auto i = num_concurrent_states; i-- > 0;)
		{
			auto thread = std::make_unique<NonThreadedRender>();
			thread->Init(this);
			_render_threads.emplace_back(std::move(thread));
		}
		_pre_render_complete = device.createSemaphoreUnique(vk::SemaphoreCreateInfo{});
		_convoluter.Init(
			Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VPBRConvolute],
			Core::GetSystem<GraphicsSystem>().renderer_geometry_shaders[GSinglePassCube],
			Core::GetSystem<GraphicsSystem>().renderer_fragment_shaders[FPBRConvolute]	
		);
	}
	void FrameRenderer::SetPipelineManager(PipelineManager& manager)
	{
		_pipeline_manager = &manager;
	}
	void FrameRenderer::PreRenderGraphicsStates(const PreRenderData& state, uint32_t frame_index)
	{
		auto& lights = *state.shared_gfx_state->lights;
		size_t num_conv_states = 1;
		auto total_pre_states = lights.size() + num_conv_states;
		GrowStates(_pre_states, total_pre_states);
		for (auto& state : _pre_states)
		{
			state.Reset();
		}
		size_t curr_state = 0;
		//Do the shadow pass here.
		for (auto light_idx : state.active_lights)
		{
			auto& light = lights[light_idx];
			auto& rs = _pre_states[curr_state++];
			PreRenderShadow(light, state, rs, frame_index);
		}
		//TODO: Submit the command buffers

		vector<vk::CommandBuffer> buffers{};


		auto& cameras = *state.cameras;
		if (cameras.size())
		{
			auto& convolute_state = _pre_states[curr_state];

			_convoluter.BeginQueue(convolute_state.ubo_manager,{});
			for (auto& camera : cameras)
			{
				std::visit([&](auto& clear)
					{
						if constexpr (std::is_same_v<std::decay_t<decltype(clear)>, RscHandle<CubeMap>>)
						{
							RscHandle<CubeMap> cubemap = clear;
							VknCubemap& cm = cubemap.as<VknCubemap>();
							RscHandle<VknCubemap> conv = cm.GetConvoluted();
							if (!conv->is_convoluted && conv != RscHandle<VknCubemap>{})
							{	
								convolute_state.FlagRendered();
								_convoluter.QueueConvoluteCubeMap(RscHandle<CubeMap>{cubemap}, RscHandle<CubeMap>{conv});
								conv->is_convoluted = true;
							}
						}
					}, camera.clear_data);
			}
			auto cmd_buffer = convolute_state.cmd_buffer;
			cmd_buffer.begin(vk::CommandBufferBeginInfo
				{
					vk::CommandBufferUsageFlagBits::eOneTimeSubmit
				}
			);
			_convoluter.ProcessQueue(cmd_buffer);
			cmd_buffer.end();

		}

		for (auto& state : _pre_states)
		{
			if (state.has_commands)
				buffers.emplace_back(state.cmd_buffer);
		}

		auto& current_signal = View().CurrPresentationSignals();

		//auto& waitSemaphores = *current_signal.image_available;
		//vk::Semaphore readySemaphores = {};///* *current_signal.render_finished; // */ *_states[0].signal.render_finished;
		//hash_set<vk::Semaphore> ready_semaphores;
		//for (auto& state : gfx_states)
		//{
		//	auto semaphore = state.camera.render_target.as<VknRenderTarget>().ReadySignal();
		//	if (semaphore)
		//		ready_semaphores.emplace(semaphore);
		//}
		//Temp, get rid of this once the other parts no longer depend on render_finished
		//ready_semaphores.emplace(readySemaphores);
		vector<vk::Semaphore> arr_ready_sem{*_pre_render_complete};
		vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eAllCommands };

		vk::SubmitInfo submit_info
		{
			0
			,nullptr
			,waitStages
			,hlp::arr_count(buffers),std::data(buffers)
			,hlp::arr_count(arr_ready_sem) ,std::data(arr_ready_sem)
		};

		auto queue = View().GraphicsQueue();
		queue.submit(submit_info, vk::Fence{}, vk::DispatchLoaderDefault{});
	}
	VulkanView& View();
	void RenderPipelineThingy(
		PipelineThingy&     the_interface    ,
		PipelineManager&    pipeline_manager ,
		vk::CommandBuffer   cmd_buffer       , 
		const vector<vec4>& clear_colors     ,
		vk::Framebuffer     frame_buffer     ,
		vk::RenderPass      rp               ,
		bool                has_depth_stencil,
		vk::Rect2D          render_area      ,
		vk::Rect2D          viewport         ,
		uint32_t            frame_index      
		)
	{
		VulkanPipeline* prev_pipeline = nullptr;
		vector<RscHandle<ShaderProgram>> shaders;

		std::array<float, 4> a{};

		//auto& cd = std::get<vec4>(state.camera.clear_data);
		//TODO grab the appropriate framebuffer and begin renderpass
		
		vector<vk::ClearValue> clear_value(clear_colors.size());
		for (size_t i = 0; i < clear_value.size(); ++i)
		{
			clear_value[i] = vk::ClearColorValue{ r_cast<const std::array<float,4>&>(clear_colors[i]) };
		}

		vk::RenderPassBeginInfo rpbi
		{
			rp, frame_buffer,
			render_area,hlp::arr_count(clear_value),std::data(clear_value)
		};


		cmd_buffer.beginRenderPass(rpbi, vk::SubpassContents::eInline);

		auto& processed_ro = the_interface.DrawCalls();
		for (auto& p_ro : processed_ro)
		{
			auto& obj = p_ro.Object();
			if (p_ro.rebind_shaders)
			{
				shaders.resize(0);
				if (p_ro.frag_shader)
					shaders.emplace_back(*p_ro.frag_shader);
				if (p_ro.vertex_shader)
					shaders.emplace_back(*p_ro.vertex_shader);
				if (p_ro.geom_shader)
					shaders.emplace_back(*p_ro.geom_shader);

				auto config = *obj.config;
				config.viewport_offset = ivec2{ s_cast<uint32_t>(viewport.offset.x),s_cast<uint32_t>(viewport.offset.y) };
				config.viewport_size = ivec2{ s_cast<uint32_t>(viewport.extent.width),s_cast<uint32_t>(viewport.extent.height) };
				auto& pipeline = pipeline_manager.GetPipeline(config,shaders,frame_index,rp,has_depth_stencil);
				pipeline.Bind(cmd_buffer, View());
				SetViewport(cmd_buffer, *config.viewport_offset, *config.viewport_size);
				prev_pipeline = &pipeline;
			}
			auto& pipeline = *prev_pipeline;
			//TODO Grab everything and render them
			//auto& mat = obj.material_instance.material.as<VulkanMaterial>();
			auto& mesh = obj.mesh.as<VulkanMesh>();
			for (auto& [set, ds] : p_ro.descriptor_sets)
			{
				cmd_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *pipeline.pipelinelayout, set, ds, {});
			}

			auto& renderer_req = *obj.renderer_req;

			for (auto&& [attrib, location] : renderer_req.requirements)
			{
				auto& attrib_buffer = mesh.Get(attrib);
				cmd_buffer.bindVertexBuffers(*pipeline.GetBinding(location), *attrib_buffer.buffer(), vk::DeviceSize{ attrib_buffer.offset }, vk::DispatchLoaderDefault{});
			}

			auto& oidx = mesh.GetIndexBuffer();
			if (oidx)
			{
				cmd_buffer.bindIndexBuffer(*(*oidx).buffer(), 0, mesh.IndexType(), vk::DispatchLoaderDefault{});
				cmd_buffer.drawIndexed(mesh.IndexCount(), 1, 0, 0, 0, vk::DispatchLoaderDefault{});
			}
		}
	}

	void FrameRenderer::PreRenderShadow(const LightData& light, const PreRenderData& state, RenderStateV2& rs, uint32_t frame_index)
	{
		auto cam = CameraData{ GenericHandle {},false, 0xFFFFFFFF,light.v,light.p };
		ShadowBinding shadow_binding;
		shadow_binding.for_each_binder<has_setstate>(
			[](auto& binder, const CameraData& cam, const vector<SkeletonTransforms>& skel)
			{
				binder.SetState(cam, skel);
			},
			cam,
				*state.skeleton_transforms);
		auto the_interface = vkn::ProcessRoUniforms(state, rs.ubo_manager, shadow_binding);
		the_interface.GenerateDS(rs.dpools);

		auto& view = View();
		//auto& swapchain = view.Swapchain();
		auto dispatcher = vk::DispatchLoaderDefault{};
		vk::CommandBuffer& cmd_buffer = rs.cmd_buffer;
		vk::CommandBufferBeginInfo begin_info{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit,nullptr };


		cmd_buffer.begin(begin_info, dispatcher);
		auto sz = light.light_map->DepthAttachment().buffer->Size();
		vk::Rect2D render_area
		{
			vk::Offset2D{},
			vk::Extent2D{s_cast<uint32_t>(sz.x),s_cast<uint32_t>(sz.y)} 
		};
		auto& rt = light.light_map.as<VknFrameBuffer>();
		vk::Framebuffer fb = rt.GetFramebuffer();
		vk::RenderPass  rp = rt.GetRenderPass ();
		rt.PrepareDraw(cmd_buffer);
		vector<vec4> clear_colors
		{
			vec4{1}
		};
		if (the_interface.DrawCalls().size())
			rs.FlagRendered();
		RenderPipelineThingy(the_interface, GetPipelineManager(), cmd_buffer, clear_colors, fb, rp, true, render_area,render_area,frame_index);

		rs.ubo_manager.UpdateAllBuffers();
		cmd_buffer.endRenderPass();
		cmd_buffer.end();

	}

	void FrameRenderer::RenderGraphicsStates(const vector<GraphicsState>& gfx_states, uint32_t frame_index)
	{
		_current_frame_index = frame_index;
		//Update all the resources that need to be updated.
		auto& curr_frame = *this;
		GrowStates(_states,gfx_states.size());
		size_t num_concurrent = curr_frame._render_threads.size();
		auto& pri_buffer = curr_frame._pri_buffer;
		auto& transition_buffer = curr_frame._transition_buffer;
		auto queue = View().GraphicsQueue();
		auto& swapchain = View().Swapchain();
		for (auto& state : curr_frame._states)
		{
			state.Reset();
		}
		bool rendered = false;
		for (size_t i = 0; i + num_concurrent <= gfx_states.size(); i += num_concurrent)
		{
			//Spawn/Assign to the threads
			for (size_t j = 0; j < num_concurrent; ++j) {
				auto& state = gfx_states[i + j];
				auto& rs = _states[i + j];
				_render_threads[j]->Render(state, rs);
				rendered = true;
				//TODO submit command buffer here and signal the framebuffer's stuff.
				//TODO create two renderpasses, detect when a framebuffer is used for the first time, use clearing renderpass for the first and non-clearing for the second onwards.
				//OR sort the gfx states so that we process all the gfx_states that target the same render target within the same command buffer/render pass.
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

		if (!rendered)
		{
			vk::ImageMemoryBarrier presentToClearBarrier = {};
			presentToClearBarrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
			presentToClearBarrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
			presentToClearBarrier.oldLayout = vk::ImageLayout::eUndefined;
			presentToClearBarrier.newLayout = vk::ImageLayout::eGeneral;
			presentToClearBarrier.srcQueueFamilyIndex = *View().QueueFamily().graphics_family;
			presentToClearBarrier.dstQueueFamilyIndex = *View().QueueFamily().graphics_family;
			presentToClearBarrier.image = swapchain.m_graphics.images[swapchain.curr_index];
			presentToClearBarrier.subresourceRange = subResourceRange;
			begin_info.pInheritanceInfo = &iinfo;
			transition_buffer->begin(begin_info, vk::DispatchLoaderDefault{});
			transition_buffer->pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, vk::DependencyFlags{}, nullptr, nullptr, presentToClearBarrier, vk::DispatchLoaderDefault{});
			transition_buffer->end();
			//hlp::TransitionImageLayout(*transition_buffer, queue, swapchain.images[swapchain.curr_index], vk::Format::eUndefined, vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR,&iinfo);
			pri_buffer->executeCommands(*transition_buffer, vk::DispatchLoaderDefault{});
		}
		pri_buffer->end();

		buffers.emplace_back(*pri_buffer);
		auto& current_signal = View().CurrPresentationSignals();

		vector<vk::Semaphore> waitSemaphores{ *current_signal.image_available, *_pre_render_complete };
		vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eAllCommands,vk::PipelineStageFlagBits::eAllCommands };
		vk::Semaphore readySemaphores =/* *current_signal.render_finished; // */ *_states[0].signal.render_finished;
		hash_set<vk::Semaphore> ready_semaphores;
		for (auto& state : gfx_states)
		{
			auto semaphore = state.camera.render_target.as<VknRenderTarget>().ReadySignal();
			if(semaphore)
			ready_semaphores.emplace(semaphore);
		}
		//Temp, get rid of this once the other parts no longer depend on render_finished
		ready_semaphores.emplace(readySemaphores);
		vector<vk::Semaphore> arr_ready_sem(ready_semaphores.begin(), ready_semaphores.end());
		auto inflight_fence = /* *current_signal.inflight_fence;// */*_states[0].signal.inflight_fence;

		//std::vector<vk::CommandBuffer> cmd_buffers;
		//for (auto& state : curr_frame.states)
		//{
		//	cmd_buffers.emplace_back(state.cmd_buffer);
		//}
		//cmd_buffers.emplace_back(*pri_buffer);
		vk::SubmitInfo submit_info
		{
			hlp::arr_count(waitSemaphores)
			,std::data(waitSemaphores)
			,waitStages
			,hlp::arr_count(buffers),std::data(buffers)
			,hlp::arr_count(arr_ready_sem) ,std::data(arr_ready_sem)
		};


		View().Device()->resetFences(1, &inflight_fence, vk::DispatchLoaderDefault{});
		queue.submit(submit_info, inflight_fence, vk::DispatchLoaderDefault{});
		View().Swapchain().m_graphics.images[frame_index] = RscHandle<VknRenderTarget>()->GetColorBuffer().as<VknTexture>().Image();
	}
	PresentationSignals& FrameRenderer::GetMainSignal()
	{
		return _states[0].signal;
	}
	void FrameRenderer::GrowStates(vector<RenderStateV2>& states, size_t new_min_size)
	{
		auto cmd_pool = *View().Commandpool();
		auto device = *View().Device();
		if (new_min_size > states.size())
		{
			auto diff = s_cast<uint32_t>(new_min_size - states.size());
			auto&& buffers = device.allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo{ cmd_pool,vk::CommandBufferLevel::ePrimary, diff }, vk::DispatchLoaderDefault{});
			for (auto i = diff; i-- > 0;)
			{
				auto& buffer = buffers[i];
				states.emplace_back(RenderStateV2{ *buffer,UboManager{View()},PresentationSignals{},DescriptorsManager{View()},CubemapRenderer{} }).signal.Init(View());
				_state_cmd_buffers.emplace_back(std::move(buffer));
			}
		}
	}
	//Assumes that you're in the middle of rendering other stuff, i.e. command buffer's renderpass has been set
	//and command buffer hasn't ended
	void FrameRenderer::RenderDebugStuff(const GraphicsState& state, RenderStateV2& rs,ivec2 vp_pos, ivec2 vp_size)
	{
		auto dispatcher = vk::DispatchLoaderDefault{};
		vk::CommandBuffer& cmd_buffer = rs.cmd_buffer;
		//TODO: figure out inheritance pipeline inheritance and inherit from dbg_pipeline for various viewport sizes
		auto& pipeline = state.dbg_pipeline;

		//Preprocess MeshRender's uniforms
		//auto&& [processed_ro, layout_count] = ProcessRoUniforms(state, rs.ubo_manager);
		//rs.ubo_manager.UpdateAllBuffers();
		//auto alloced_dsets = rs.dpools.Allocate(layout_count);
		rs.FlagRendered();
		pipeline->Bind(cmd_buffer, *_view);
		SetViewport(cmd_buffer, vp_pos, vp_size);
		//Bind the uniforms
		auto& layouts = pipeline->uniform_layouts;
		uint32_t trf_set = 0;
		auto itr = layouts.find(trf_set);
		if (itr != layouts.end())
		{
			auto ds_layout = itr->second;
			auto allocated =rs.dpools.Allocate(hash_table<vk::DescriptorSetLayout, std::pair<vk::DescriptorType, uint32_t>>{ {ds_layout, {vk::DescriptorType::eUniformBuffer,2}}});
			auto aitr = allocated.find(ds_layout);
			if (aitr != allocated.end())
			{
				auto&& [view_buffer, vb_offset] = rs.ubo_manager.Add(state.camera.view_matrix);
				auto&& [proj_buffer, pb_offset] = rs.ubo_manager.Add(mat4{ 1,0,0,0,   0,1,0,0,   0,0,0.5f,0.5f, 0,0,0,1 }*state.camera.projection_matrix);
				auto ds = aitr->second.GetNext();
				UpdateUniformDS(*View().Device(), ds, vector<ProcessedRO::BindingInfo>{ 
					ProcessedRO::BindingInfo{
						0,view_buffer,vb_offset,0,sizeof(mat4),itr->second
					},
					ProcessedRO::BindingInfo{ 1,proj_buffer,pb_offset,0,sizeof(mat4),itr->second }
				});
				cmd_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *pipeline->pipelinelayout, 0, ds, {});
			}
		}

		const DbgDrawCall* prev = nullptr;
		for (auto& p_dc : state.dbg_render)
		{
			auto& dc = *p_dc;
			dc.Bind(cmd_buffer,prev);
			dc.Draw(cmd_buffer);
			prev = p_dc;
			
		}
	}
	vk::RenderPass FrameRenderer::GetRenderPass(const GraphicsState& state, VulkanView&)
	{
		//vk::RenderPass result = view.BasicRenderPass(BasicRenderPasses::eRgbaColorDepth);
		//if (state.camera.is_shadow)
		//	result = view.BasicRenderPass(BasicRenderPasses::eDepthOnly);
		return state.camera.render_target.as<VknRenderTarget>().GetRenderPass(state.clear_render_target && state.camera.clear_data.index() != meta::IndexOf <std::remove_const_t<decltype(state.camera.clear_data)>,DontClear>::value);
	}


	void TransitionFrameBuffer(const CameraData& camera, vk::CommandBuffer cmd_buffer, VulkanView& )
	{
		auto& vkn_fb = camera.render_target.as<VknRenderTarget>();
		vkn_fb.PrepareDraw(cmd_buffer);
	}


	pipeline_config ConfigWithVP(pipeline_config config, const CameraData& camera, const ivec2& offset, const ivec2& size)
	{
		config.render_pass_type = camera.render_target.as<VknRenderTarget>().GetRenderPassType();
		config.viewport_offset = offset;
		config.viewport_size = size ;
		return config;
	}

	void CopyDepthBuffer(vk::CommandBuffer cmd_buffer, vk::Image rtd, vk::Image gd)
	{
		//Transit RTD -> Transfer
		vk::ImageMemoryBarrier depth
		{
			{},
			vk::AccessFlagBits::eTransferWrite,
			vk::ImageLayout::eGeneral,
			vk::ImageLayout::eTransferDstOptimal,
			*View().QueueFamily().graphics_family,
			*View().QueueFamily().graphics_family,
			rtd,
			vk::ImageSubresourceRange
			{
				vk::ImageAspectFlagBits::eDepth,
				0,1,0,1
			}
		};
		cmd_buffer.pipelineBarrier(vk::PipelineStageFlagBits::eBottomOfPipe, vk::PipelineStageFlagBits::eTransfer, vk::DependencyFlagBits::eByRegion, nullptr, nullptr, depth);

		//Blit
		//expect depth attachments to be the exact same size and format.
		cmd_buffer.copyImage(
			gd, vk::ImageLayout::eTransferSrcOptimal,
			rtd, vk::ImageLayout::eTransferDstOptimal,
			vk::ImageCopy
			{
				vk::ImageSubresourceLayers
				{
					vk::ImageAspectFlagBits::eDepth,
					0u,0u,1u
				},
				vk::Offset3D{0,0,0},
				vk::ImageSubresourceLayers
				{
					vk::ImageAspectFlagBits::eDepth,
					0ui32,0ui32,1ui32
				},
				vk::Offset3D{0,0,0}
			}
		);


		//Transit RTD -> General
		std::array<vk::ImageMemoryBarrier, 2> return_barriers = { depth,depth };
		std::swap(return_barriers[0].dstAccessMask, return_barriers[0].srcAccessMask);
		std::swap(return_barriers[0].oldLayout, return_barriers[0].newLayout);
		return_barriers[0].setDstAccessMask({});// vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite);
		return_barriers[0].setNewLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

		//Transit GD  -> DepthAttachmentOptimal
		return_barriers[1].setSrcAccessMask(vk::AccessFlagBits::eTransferRead);
		return_barriers[1].setDstAccessMask({});
		return_barriers[1].setOldLayout(vk::ImageLayout::eTransferSrcOptimal);
		return_barriers[1].setNewLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
		return_barriers[1].setImage(gd);

		cmd_buffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eBottomOfPipe, vk::DependencyFlagBits::eByRegion, nullptr, nullptr, return_barriers);

	}

	void FrameRenderer::RenderGraphicsState(const GraphicsState& state, RenderStateV2& rs)
	{
		auto& view = View();
		//auto& swapchain = view.Swapchain();
		auto& camera = state.camera;
		auto dispatcher = vk::DispatchLoaderDefault{};
		vk::CommandBuffer& cmd_buffer = rs.cmd_buffer;
		vk::CommandBufferBeginInfo begin_info{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit,nullptr };

		VulkanPipeline* prev_pipeline = nullptr;
		vector<RscHandle<ShaderProgram>> shaders;
		auto& deferred_pass = rs.deferred_pass;
		deferred_pass.fullscreen_quad_vert = Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VertexShaders::VFsq];
		deferred_pass.deferred_post_frag = Core::GetSystem<GraphicsSystem>().renderer_fragment_shaders[FragmentShaders::FDeferredPost];
		deferred_pass.Init(state.camera.render_target->Size());
		deferred_pass.pipeline_manager(GetPipelineManager());
		deferred_pass.frame_index(_current_frame_index);

		cmd_buffer.begin(begin_info, dispatcher);

		//Preprocess MeshRender's uniforms
		deferred_pass.DrawToGBuffers(cmd_buffer, state, rs);
		auto rtd = camera.render_target->GetDepthBuffer().as<VknTexture>().Image();
		auto gd = deferred_pass.GBuffer().gbuffer->DepthAttachment().buffer.as<VknTexture>().Image();
		
		//cmd_buffer.blitImage(
		//	deferred_pass.GBuffer().gbuffer->DepthAttachment().buffer.as<VknTexture>().Image(), vk::ImageLayout::eTransferSrcOptimal,
		//	camera.render_target->GetDepthBuffer().as<VknTexture>().Image(), vk::ImageLayout::eTransferDstOptimal,
		//	blit, vk::Filter::eNearest
		//);


		auto&& the_interface = rs.deferred_pass.ProcessDrawCalls(state, rs);//ProcessRoUniforms(state, rs.ubo_manager);
		the_interface.GenerateDS(rs.dpools);
		std::array<float, 4> a{};

		//auto& cd = std::get<vec4>(state.camera.clear_data);
		//TODO grab the appropriate framebuffer and begin renderpass
		std::array<float, 4> depth_clear{ 1.0f,1.0f ,1.0f ,1.0f };
		std::optional<color> clear_col;
		std::optional<RscHandle<CubeMap>> sb_cm;

		auto& clear_data = state.camera.clear_data;
		switch (clear_data.index())
		{
		case index_in_variant_v<color, CameraClear_t>:
			clear_col = std::get<color>(clear_data);
			break;
		case index_in_variant_v<RscHandle<CubeMap>, CameraClear_t>:
			sb_cm = std::get<RscHandle<CubeMap>>(clear_data);
			break;
		case index_in_variant_v<DontClear, CameraClear_t>:
			//TODO: set dont clear settings.
			break;
		}


		vk::ClearValue clearColor = clear_col ?
			vk::ClearValue{ vk::ClearColorValue{ r_cast<const std::array<float,4>&>(clear_col) } }
			:
			vk::ClearValue{ vk::ClearColorValue{ std::array < float, 4>{0.f,0.f,0.f,0.f} } };
		vk::ClearValue v[]{
			clearColor,
			vk::ClearValue {vk::ClearColorValue{ depth_clear }}
		};
		
		//auto& vvv = state.camera.render_target.as<VknFrameBuffer>();
		
		//auto default_frame_buffer = *swapchain.frame_buffers[swapchain.curr_index];
		auto& vkn_fb = camera.render_target.as<VknRenderTarget>();
		auto frame_buffer = GetFrameBuffer(camera, view.CurrFrame());
		TransitionFrameBuffer(camera, cmd_buffer, view);

		auto sz = camera.render_target->Size();
		auto [offset, size] = ComputeVulkanViewport(vec2{ sz }, camera.viewport);
		

		vk::Rect2D render_area
		{
			vk::Offset2D
			{
				s_cast<int32_t>(offset.x),s_cast<int32_t>(offset.y)
			},vk::Extent2D
			{
				s_cast<uint32_t>(size.x),s_cast<uint32_t>(size.y)
			}
		};
		vk::RenderPassBeginInfo rpbi
		{
			GetRenderPass(state,view), frame_buffer,
			render_area,hlp::arr_count(v),std::data(v)
		};			

		cmd_buffer.beginRenderPass(rpbi, vk::SubpassContents::eInline, dispatcher);
		//////////////////Skybox rendering
		if (sb_cm)
		{
			auto& vknCubeMap = sb_cm->as<VknCubemap>();
			pipeline_config skybox_render_config;
			DescriptorsManager skybox_ds_manager(view);
			skybox_render_config.fill_type = FillType::eFill;
			skybox_render_config.prim_top = PrimitiveTopology::eTriangleList;
			auto config = ConfigWithVP(skybox_render_config,camera,offset,size);
			config.vert_shader = Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VSkyBox];
			config.frag_shader = Core::GetSystem<GraphicsSystem>().renderer_fragment_shaders[FSkyBox];
			config.cull_face = s_cast<uint32_t>(CullFace::eNone);
			config.depth_test = false;
			config.render_pass_type = BasicRenderPasses::eRgbaColorDepth;

			//No idea if this is expensive....if really so I will try shift up to init
			rs.skyboxRenderer.Init(
				Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VSkyBox],
				{},
				Core::GetSystem<GraphicsSystem>().renderer_fragment_shaders[FSkyBox],
				&config,
				*camera.CubeMapMesh
			);
			rs.skyboxRenderer.QueueSkyBox(rs.ubo_manager, {}, *sb_cm, camera.projection_matrix * camera.view_matrix);
			
			/*cmd_buffer.begin(vk::CommandBufferBeginInfo
				{
					vk::CommandBufferUsageFlagBits::eOneTimeSubmit
				});*/
			rs.skyboxRenderer.ProcessQueueWithoutRP(cmd_buffer, offset, size);
		}

		auto& processed_ro = the_interface.DrawCalls();
		rs.FlagRendered();
		bool first_time = true;
		for (auto& p_ro : processed_ro)
		{
			auto& obj = p_ro.Object();
			if (p_ro.rebind_shaders)
			{
				shaders.resize(0);
				if (p_ro.frag_shader)
					shaders.emplace_back(*p_ro.frag_shader);
				if(p_ro.vertex_shader)
					shaders.emplace_back(*p_ro.vertex_shader);
				if(p_ro.geom_shader)
					shaders.emplace_back(*p_ro.geom_shader);

				auto config = ConfigWithVP(*obj.config, camera, offset, size);
				auto& pipeline = GetPipeline(config, shaders);
				pipeline.Bind(cmd_buffer,view);
				SetViewport(cmd_buffer, offset, size);
				prev_pipeline = &pipeline;
			}
			auto& pipeline = *prev_pipeline;
			//TODO Grab everything and render them
			//auto& mat = obj.material_instance.material.as<VulkanMaterial>();
			auto& mesh = obj.mesh.as<VulkanMesh>();
			for (auto& [set,ds] : p_ro.descriptor_sets)
			{
				cmd_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *pipeline.pipelinelayout, set, ds, {});
			}
			
			auto& renderer_req = *obj.renderer_req;
			
			for (auto&& [attrib, location] : renderer_req.requirements)
			{
				auto& attrib_buffer = mesh.Get(attrib);
				cmd_buffer.bindVertexBuffers(*pipeline.GetBinding(location), *attrib_buffer.buffer(), vk::DeviceSize{ attrib_buffer.offset }, vk::DispatchLoaderDefault{});
			}
			
			auto& oidx = mesh.GetIndexBuffer();
			if (oidx)
			{
				cmd_buffer.bindIndexBuffer(*(*oidx).buffer(), 0, mesh.IndexType(), vk::DispatchLoaderDefault{});
				cmd_buffer.drawIndexed(mesh.IndexCount(), 1, 0, 0, 0, vk::DispatchLoaderDefault{});
			}
			if (first_time)
			{
				CopyDepthBuffer(cmd_buffer, rtd, gd);
			}
			first_time = false;
		}
		if(camera.overlay_debug_draw)
			RenderDebugStuff(state, rs,offset,size);
		rs.ubo_manager.UpdateAllBuffers();
		cmd_buffer.endRenderPass();
		cmd_buffer.end();
		Track(0);
	}

	PipelineManager& FrameRenderer::GetPipelineManager()
	{
		return *_pipeline_manager;
	}

	VulkanPipeline& FrameRenderer::GetPipeline(const pipeline_config& config,const vector<RscHandle<ShaderProgram>>& modules)
	{
		// TODO: Replace with something that actually gets the pipeline
		return GetPipelineManager().GetPipeline(config,modules,_current_frame_index);
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