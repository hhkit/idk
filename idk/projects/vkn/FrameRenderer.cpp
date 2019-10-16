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
#include <vkn/VknRenderTarget.h>
#include <gfx/Light.h>

#include <gfx/MeshRenderer.h>
#include <anim/SkinnedMeshRenderer.h>
#include <vkn/vulkan_enum_info.h>
#include <vkn/VknTexture.h>
#include <vkn/VulkanHashes.h>



namespace idk::vkn
{
	using collated_bindings_t = hash_table < uint32_t, vector<ProcessedRO::BindingInfo>>;//Set, bindings


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


	struct FrameRenderer::VertexUniformConfig
	{
		RscHandle<ShaderProgram> _shader;
		//template<typename T= FrameRenderer>
		struct Ref
		{
			using CollatedLayouts_t = DsBindingCount;
			CollatedLayouts_t& collated_layouts;
			UboManager& ubo_manager;
			Ref(CollatedLayouts_t* cl = nullptr, UboManager* u = nullptr) :collated_layouts{ *cl }, ubo_manager{ *u }{}
		};
		Ref ref;
		void SetRef(
			DsBindingCount& collated_layouts,
			//set, bindings
			UboManager& ubo_manager)
		{
			new (&ref) Ref{ &collated_layouts,&ubo_manager };
		}
		ShaderModule& Shader()const
		{
			return _shader.as<ShaderModule>();
		}
		RscHandle<ShaderModule> ShaderHandle()const
		{
			return RscHandle<ShaderModule>{_shader};
		}
		//struct ExBindingInfo
		//{
		//	ProcessedRO::BindingInfo info;
		//	vk::DescriptorSetLayout layout;
		//	operator ProcessedRO::BindingInfo& ()
		//	{
		//		return info;
		//	}
		//	ExBindingInfo(const ProcessedRO::BindingInfo& i, vk::DescriptorSetLayout l) : info{i},layout{l}
		//	{
		//	}
		//};

		vector<std::pair<uint32_t, ProcessedRO::BindingInfo>> binding_infos;//set, binding_info
		template<typename T>
		void RegisterBindingInfo(const string& name, const T& data)
		{
			auto& shader = Shader();
			auto& uni_info = shader.GetLayout(name);
			binding_infos.emplace_back(uni_info.set, CreateBindingInfo(uni_info, data, ref.collated_layouts, ref.ubo_manager));
		}
		template<typename Tracker>
		void BindRegistered(collated_bindings_t& collated_bindings,Tracker&& tracker)
		{
			for (auto& info : binding_infos)
			{
				collated_bindings[info.first].emplace_back(info.second);
				tracker(info.second.layout, ref.collated_layouts);
			}
		}
		template<typename T, typename Tracker>
		void BindOnce(collated_bindings_t& collated_bindings, const string& name, const T& data, Tracker tracker)
		{
			auto& shader = Shader();
			auto& uni_info = shader.GetLayout(name);
			collated_bindings[uni_info.set].emplace_back(CreateBindingInfo(uni_info, data, ref.collated_layouts, ref.ubo_manager));
			tracker(uni_info.layout,ref.collated_layouts);
		}
		VertexUniformConfig()
		{
		}
	};
	void BindCameraStuff(PipelineThingy& the_interface, const CameraData& cam)
	{
		//map back into z: (0,1)
		mat4 projection_trf = mat4{ 1,0,0,0,
							0,1,0,0,
							0,0,0.5f,0.5f,
							0,0,0,1
		}*cam.projection_matrix;//map back into z: (0,1)
		the_interface.BindUniformBuffer("CameraBlock", 0, projection_trf);
	}

	string PrepareLightBlock(const CameraData& cam, const vector<LightData>& lights)
	{
		vector<BaseLightData> tmp_light(lights.size());
		for (size_t i = 0; i < tmp_light.size(); ++i)
		{
			auto& light = tmp_light[i] = (lights)[i];
			light.v_pos = cam.view_matrix * vec4{ light.v_pos,1 };
			light.v_dir = (cam.view_matrix * vec4{ light.v_dir,0 }).get_normalized();
		}

		string light_block;
		uint32_t len = s_cast<uint32_t>(tmp_light.size());
		light_block += string{ reinterpret_cast<const char*>(&len),sizeof(len) };
		light_block += string(16 - sizeof(len), '\0');
		light_block += string{ reinterpret_cast<const char*>(tmp_light.data()), hlp::buffer_size(tmp_light) };
		return light_block;
	}
	struct StandardBindings
	{
		virtual void Bind([[maybe_unused]]PipelineThingy& the_interface) {}
		virtual void Bind([[maybe_unused]]PipelineThingy& the_interface, [[maybe_unused]] const  RenderObject& dc) {}
		void Bind(PipelineThingy& the_interface, AnimatedRenderObject& dc)
		{
			Bind(the_interface, s_cast<RenderObject&>(dc));
			BindAni(the_interface, dc);
		}
		virtual void BindAni([[maybe_unused]] PipelineThingy& the_interface, [[maybe_unused]] const AnimatedRenderObject& dc) {}
	};

	struct StandardVertexBindings : StandardBindings
	{
		const GraphicsState* _state;
		const GraphicsState& State() {return *_state;}
		mat4 view_trf, proj_trf;
		void SetState(const GraphicsState& vstate) {
			_state = &vstate;
			auto& state = State();
			auto& cam = state.camera;
			view_trf = cam.view_matrix;
			proj_trf = cam.projection_matrix;
		}

		void Bind(PipelineThingy& the_interface)override
		{
			auto& state = State();
			auto& cam = state.camera;
			BindCameraStuff(the_interface, cam);
		}
		void Bind(PipelineThingy& the_interface, const RenderObject& dc)override
		{
			mat4 obj_trf = view_trf * dc.transform;
			mat4 obj_ivt = obj_trf.inverse().transpose();
			vector<mat4> mat4_block{ obj_trf,obj_ivt };
			the_interface.BindUniformBuffer("ObjectMat4Block", 0, mat4_block);
		}
		void Bind(PipelineThingy& the_interface, const  AnimatedRenderObject& dc)
		{
			Bind(the_interface, s_cast<const RenderObject&>(dc));
			BindAni(the_interface, dc);
		}
		virtual void BindAni(PipelineThingy& the_interface, const AnimatedRenderObject& dc)
		{
			auto& state = State();
			the_interface.BindUniformBuffer("BoneMat4Block", 0, state.GetSkeletonTransforms()[dc.skeleton_index].bones_transforms);
		}

	};
	struct PbrFwdBindings : StandardBindings
	{
		const GraphicsState* _state;
		CameraData cam;
		const GraphicsState& State() { return *_state; }
		string light_block;
		mat4 view_trf, pbr_trf, proj_trf;
		void SetState(const GraphicsState& vstate) {
			_state = &vstate;
			auto& state = State();
			cam = state.camera;
			light_block = PrepareLightBlock(cam, *state.lights);
			pbr_trf = view_trf.inverse();
		}

		virtual void Bind(PipelineThingy& the_interface, const RenderObject& dc)
		{
			auto& state = State();
			the_interface.BindUniformBuffer("LightBlock", 0, light_block, true);//skip if pbr is already bound(not per instance)
			the_interface.BindUniformBuffer("PBRBlock", 0, pbr_trf, true);//skip if pbr is already bound(not per instance)
			uint32_t i = 0;
			//Bind the shadow maps
			for (auto& shadow_map : state.active_lights)
			{
				auto& sm_uni = shadow_map->light_map;
				{
					auto hdepth_tex = sm_uni->GetDepthBuffer();
					auto& depth_tex = hdepth_tex.as<VknTexture>();
					the_interface.BindSampler("shadow_maps", i++, depth_tex, true);
				}
			}
		}
	};

	struct StandardMaterialFragBindings : StandardBindings
	{
		//Assumes that the material is valid.
		void Bind(PipelineThingy& the_interface, const  RenderObject& dc)override
		{
			auto& mat_inst = *dc.material_instance;
			auto& mat = *mat_inst.material;
			the_interface.BindShader(ShaderStage::Fragment, mat._shader_program);
		}

	};
	struct StandardMaterialBindings : StandardBindings
	{
		const GraphicsState* _state;
		const GraphicsState& State() { return *_state; }
		void SetState(const GraphicsState& vstate) {
			_state = &vstate;
			auto& state = State();
		}

		//Assumes that the material is valid.
		virtual void Bind(PipelineThingy& the_interface, const  RenderObject& dc)
		{
			//Bind the material uniforms
			{
				auto& mat_inst = *dc.material_instance;
				auto& mat = *mat_inst.material;
				auto mat_cache = mat_inst.get_cache();
				for (auto itr = mat_cache.uniforms.begin(); itr != mat_cache.uniforms.end(); ++itr)
				{
					if (mat_cache.IsUniformBlock(itr))
					{
						the_interface.BindUniformBuffer(itr->first, 0, mat_cache.GetUniformBlock(itr));
					}
					else if (mat_cache.IsImageBlock(itr))
					{
						auto img_block = mat_cache.GetImageBlock(itr);
						uint32_t i = 0;
						for (auto& img : img_block)
						{
							the_interface.BindSampler(itr->first.substr(0, itr->first.find_first_of('[')), i++, img.as<VknTexture>());
						}
					}
				}
			}
		}

		virtual void BindAni(PipelineThingy& the_interface, const AnimatedRenderObject& dc)
		{
		}

	};
	namespace detail
	{
		template<template<typename,typename...>typename cond,typename ...FArgs>
		struct for_each_binder_impl
		{
			template<typename T>
			using cond2 = cond<T,FArgs...>;
			template<typename T>
			using cond_bool =meta::SfinaeBool<cond2, T>;
		};
		
	}
	template<typename ...Args>
	struct CombinedBindings :StandardBindings
	{
		std::tuple<Args...> binders;
		template<template<typename...>typename cond = meta::always_true_va , typename Fn, typename ...FArgs >
		void for_each_binder(Fn&& f, FArgs& ...args)
		{
			meta::for_each_tuple_element<typename detail::for_each_binder_impl<cond,FArgs...>::cond_bool>(binders,
				[](auto& binder, auto& func, auto& ...args) 
				{
					func(binder, args...);
				}
			, f, args...);
		}
		void Bind(PipelineThingy& the_interface) override
		{
			meta::for_each_tuple_element(binders, [](StandardBindings& binder, auto& the_interface)
				{
					binder.Bind(the_interface); 
				}, the_interface);
		}
		void Bind(PipelineThingy& the_interface, const RenderObject& dc) override
		{
			meta::for_each_tuple_element(binders, [](StandardBindings& binder, auto& the_interface, auto& dc) {
				binder.Bind(the_interface,dc);
				}, the_interface,dc);
		}
		void BindAni(PipelineThingy& the_interface, const  AnimatedRenderObject& dc) override
		{
			meta::for_each_tuple_element(binders, [](StandardBindings& binder, auto& the_interface, auto& dc) {
				binder.BindAni(the_interface, dc);
				}, the_interface, dc);
		}
	};

	using PbrFwdMaterialBinding = CombinedBindings<StandardVertexBindings,StandardMaterialFragBindings, PbrFwdBindings, StandardMaterialBindings>;
	using ShadowBinding = CombinedBindings<StandardVertexBindings>;


	PipelineThingy ProcessRoUniforms(const GraphicsState& state, UboManager& ubo_manager,StandardBindings* binder)
	{
		auto& binders = *binder;
		PipelineThingy the_interface{};
		the_interface.SetRef(ubo_manager);

		the_interface.BindShader(ShaderStage::Vertex, state.mesh_vtx);
		binders.Bind(the_interface);
		{
			const vector<const RenderObject*>& draw_calls = state.mesh_render;
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
			const vector<const AnimatedRenderObject*>& draw_calls = state.skinned_mesh_render;
			the_interface.BindShader(ShaderStage::Vertex, state.skinned_mesh_vtx);
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
	template<typename T,typename...Args>
	using has_setstate = decltype(std::declval<T>().SetState(std::declval<Args>()...));
	//Possible Idea: Create a Pipeline object that tracks currently bound descriptor sets
	PipelineThingy FrameRenderer::ProcessRoUniforms(const GraphicsState& state, UboManager& ubo_manager)
	{
		if (state.camera.is_shadow)
		{
			ShadowBinding binders;
			binders.for_each_binder([](auto& binder, const GraphicsState& state) {binder.SetState(state); }, state);
			return vkn::ProcessRoUniforms(state, ubo_manager, &binders);
		}
		else
		{
			PbrFwdMaterialBinding binders;
			binders.for_each_binder<has_setstate>([](auto& binder, const GraphicsState& state) {binder.SetState(state); }, state);
			return vkn::ProcessRoUniforms(state,ubo_manager,&binders);

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

		GrowStates(num_concurrent_states);
		//Temp
		for (auto i = num_concurrent_states; i-- > 0;)
		{
			auto thread = std::make_unique<NonThreadedRender>();
			thread->Init(this);
			_render_threads.emplace_back(std::move(thread));
		}


	}
	void FrameRenderer::SetPipelineManager(PipelineManager& manager)
	{
		_pipeline_manager = &manager;
	}
	void FrameRenderer::RenderGraphicsStates(const vector<GraphicsState>& gfx_states, uint32_t frame_index)
	{
		_current_frame_index = frame_index;
		//Update all the resources that need to be updated.
		auto& curr_frame = *this;
		GrowStates(gfx_states.size());
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

		auto& waitSemaphores = *current_signal.image_available;
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
	//Assumes that you're in the middle of rendering other stuff, i.e. command buffer's renderpass has been set
	//and command buffer hasn't ended
	void FrameRenderer::RenderDebugStuff(const GraphicsState& state, RenderStateV2& rs)
	{
		auto dispatcher = vk::DispatchLoaderDefault{};
		vk::CommandBuffer& cmd_buffer = rs.cmd_buffer;
		auto& pipeline = *state.dbg_pipeline;
		//Preprocess MeshRender's uniforms
		//auto&& [processed_ro, layout_count] = ProcessRoUniforms(state, rs.ubo_manager);
		//rs.ubo_manager.UpdateAllBuffers();
		//auto alloced_dsets = rs.dpools.Allocate(layout_count);
		rs.FlagRendered();
		pipeline.Bind(cmd_buffer, *_view);
		//Bind the uniforms
		auto& layouts = pipeline.uniform_layouts;
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
				cmd_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *pipeline.pipelinelayout, 0, ds, {});
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
		return state.camera.render_target.as<VknRenderTarget>().GetRenderPass();
	}


	void TransitionFrameBuffer(const CameraData& camera, vk::CommandBuffer cmd_buffer, VulkanView& )
	{
		auto& vkn_fb = camera.render_target.as<VknRenderTarget>();
		vkn_fb.PrepareDraw(cmd_buffer);
	}




	void FrameRenderer::RenderGraphicsState(const GraphicsState& state, RenderStateV2& rs)
	{
		auto& view = View();
		//auto& swapchain = view.Swapchain();
		auto dispatcher = vk::DispatchLoaderDefault{};
		vk::CommandBuffer& cmd_buffer = rs.cmd_buffer;
		vk::CommandBufferBeginInfo begin_info{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit,nullptr };

		VulkanPipeline* prev_pipeline = nullptr;
		vector<RscHandle<ShaderProgram>> shaders;

		//Preprocess MeshRender's uniforms
		auto&& the_interface = ProcessRoUniforms(state, rs.ubo_manager);
		the_interface.GenerateDS(rs.dpools);

		cmd_buffer.begin(begin_info, dispatcher);
		std::array<float, 4> a{};

		//auto& cd = std::get<vec4>(state.camera.clear_data);
		//TODO grab the appropriate framebuffer and begin renderpass
		std::array<float, 4> depth_clear{1.0f,1.0f ,1.0f ,1.0f };
		std::optional<vec4> clear_col;
		std::visit([&state, &clear_col](auto clear_data)
			{
				if constexpr (std::is_same_v<decltype(clear_data), vec4>)
					clear_col = clear_data;
			}, state.camera.clear_data);
		vk::ClearValue v[]{ 
			vk::ClearValue {vk::ClearColorValue{ r_cast<const std::array<float,4>&>(clear_col) }},
			vk::ClearValue {vk::ClearColorValue{ depth_clear }}
		};
		
		//auto& vvv = state.camera.render_target.as<VknFrameBuffer>();
		
		auto& camera = state.camera;
		//auto default_frame_buffer = *swapchain.frame_buffers[swapchain.curr_index];
		auto& vkn_fb = camera.render_target.as<VknRenderTarget>();
		auto frame_buffer = GetFrameBuffer(camera, view.CurrFrame());
		TransitionFrameBuffer(camera, cmd_buffer, view);

		auto sz = camera.render_target->GetMeta().size;
		vk::Rect2D render_area
		{
			vk::Offset2D{},vk::Extent2D
			{
				s_cast<uint32_t>(sz.x),s_cast<uint32_t>(sz.y)
			}
		};
		vk::RenderPassBeginInfo rpbi
		{
			GetRenderPass(state,view), frame_buffer,
			render_area,hlp::arr_count(v),std::data(v)
		};


		cmd_buffer.beginRenderPass(rpbi, vk::SubpassContents::eInline, dispatcher);

		auto& processed_ro = the_interface.draw_calls;
		rs.FlagRendered();
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

				auto config = *obj.config;
				config.render_pass_type = camera.render_target.as<VknRenderTarget>().GetRenderPassType();
				config.screen_size = sz;
				auto& pipeline = GetPipeline(config, shaders);
				pipeline.Bind(cmd_buffer,view);
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
		}
		if(camera.overlay_debug_draw)
			RenderDebugStuff(state, rs);
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