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
#include <math/matrix_transforms.inl>
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

#include <vkn/VknCubemap.h>

#include <gfx/ViewportUtil.h>
#include <vkn/VknCubeMapLoader.h>
#include <vkn/VulkanCbmLoader.h>
#include <ui/Canvas.h>

#include <vkn/vector_buffer.h>

#include <cstdint>

#include <vkn/DescriptorUpdateData.h>
#include <thread>
#include <mutex>
#include <queue>
#include <parallel/ThreadPool.h>
#include <vkn/PipelineBinders.inl>
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>
#include <vkn/UboManager.inl>
#include <math/color.inl>
#include <ds/result.inl>

#include <vkn/DebugUtil.h>

#include <vkn/ColorPickRenderer.h>

namespace idk::vkn
{
#define CreateRenderThread() std::make_unique<ThreadedRender>()

	struct FrameRenderer::PImpl
	{
		hash_table<RscHandle<Texture>, RscHandle<VknFrameBuffer>> deferred_buffers;
		vector<ColorPickRequest> color_pick_requests;
		ColorPickRenderer color_picker;
	};

	//from: https://riptutorial.com/cplusplus/example/30142/semaphore-cplusplus-11
	class Semaphore {
	public:
		Semaphore(int count_ = 0)
			: count(count_)
		{
		}

		inline void notify() {
			std::unique_lock<std::mutex> lock(mtx);
			count++;
			//notify the waiting thread
			cv.notify_one();
		}
		inline void wait() {
			std::unique_lock<std::mutex> lock(mtx);
			while (count == 0) {
				//wait on the mutex until notify is called
				cv.wait(lock);
			}
			count--;
		}
	private:
		std::mutex mtx;
		std::condition_variable cv;
		int count;
	};
	/*
	template<typename job_t>
	struct job_queue
	{
		using container_t = std::list<job_t>;
		using iterator_t = typename container_t::iterator;
		container_t container;
		iterator_t begin, end;
		std::mutex fml;
		job_queue() :begin{ container.begin() }, end{ container.end() }{}
		void push(job_t job)
		{
			fml.lock();
			if (container.end() == end)
			{
				container.push_back(std::move(job));
				end = container.end();
				if (begin == container.end())
					begin = container.begin();
			}
			else
				new (&(*end++)) job_t{ std::move(job) };
			if (begin == container.end())
			{
				begin = end;
				--begin;
			}
			fml.unlock();
		}
		job_t pop()
		{
			fml.lock();
			job_t result = std::move(*begin);
			begin->~job_t();
			++begin;
			fml.unlock();
			return result;
		}
		bool empty()const
		{
			return begin == end;
		}
		void reset()
		{
			begin = end = container.begin();
		}
	};*/

	template<typename RT, typename ...Args>
	auto GetFuture(RT(*func)(Args...)) -> decltype(Core::GetThreadPool().Post(func, std::declval<Args>()...));


	class FrameRenderer::ThreadedRender : public FrameRenderer::IRenderThread
	{
	public:
		static void RunFunc(FrameRenderer* _renderer, const GraphicsState* m, RenderStateV2* rs,std::atomic<int>*counter) noexcept
		{
			try
			{
				(*counter)--;
				_renderer->RenderGraphicsState(*m, *rs);
			}
			catch (...)
			{

			}
		}
		using Future_t =decltype(GetFuture(ThreadedRender::RunFunc));
		void Init(FrameRenderer* renderer)
		{
			
			_renderer = renderer;
		}
		void Render(const GraphicsState& state, RenderStateV2& rs)override
		{
			counter++;
			futures.emplace_back(Core::GetThreadPool().Post(&RunFunc, _renderer, &state, &rs,&counter));
		}
//#pragma optimize("",off)
		void Join() override
		{
			for(auto& future : futures)
				future.get();
			futures.clear();
		}
	private:
		FrameRenderer* _renderer;
		std::atomic<int> counter{};
		vector<Future_t> futures;
		
		std::thread my_thread;
	};

	using collated_bindings_t = hash_table < uint32_t, vector<ProcessedRO::BindingInfo>>;//Set, bindings
	std::pair<ivec2, ivec2> ComputeVulkanViewport(const vec2& sz, const rect& vp)
	{
		auto pair = ComputeViewportExtents(sz, vp);
		auto& [offset, size] = pair;
		//offset.y = sz.y - offset.y - size.y;
		//offset = ivec2{ (translate(ivec2{ 0,sz.y }) * tmat<int,3,3> { scale(ivec2{ 1,-1 }) }).transpose()* ivec3 { offset,1 } };//  -ivec2{ 0,size.y };
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
		LayerMask mask = ~(LayerMask{});
		array<RscHandle<ShaderProgram>, VertexShaders::VMax>   renderer_vertex_shaders;
		array<RscHandle<ShaderProgram>, FragmentShaders::FMax>   renderer_fragment_shaders;
		const vector<const RenderObject*>*         mesh_render;
		const vector<const AnimatedRenderObject*>* skinned_mesh_render;
		const vector<InstRenderObjects>* inst_ro;
		std::variant<GraphicsSystem::RenderRange, GraphicsSystem::LightRenderRange> range;
		const SharedGraphicsState* shared_state = {};
		GraphicsStateInterface() = default;

		GraphicsStateInterface(const CoreGraphicsState& state)
		{
			mesh_render = &state.mesh_render;
			skinned_mesh_render = &state.skinned_mesh_render;
			shared_state = state.shared_gfx_state;
			inst_ro = state.shared_gfx_state->instanced_ros;
		}
		GraphicsStateInterface(const GraphicsState& state) : GraphicsStateInterface{static_cast<const CoreGraphicsState&>(state)}
		{
			renderer_vertex_shaders =   state.shared_gfx_state->renderer_vertex_shaders;
			renderer_fragment_shaders = state.shared_gfx_state->renderer_fragment_shaders;
			range = state.range;

			mask = state.camera.culling_flags;
		}
		GraphicsStateInterface(const PreRenderData& state) : GraphicsStateInterface{ static_cast<const CoreGraphicsState&>(state) }
		{
			renderer_vertex_shaders   = state.shared_gfx_state->renderer_vertex_shaders;
			renderer_fragment_shaders = state.shared_gfx_state->renderer_fragment_shaders;
		}
		GraphicsStateInterface(const PostRenderData& state) : GraphicsStateInterface{ static_cast<const CoreGraphicsState&>(state) }
		{
			renderer_vertex_shaders   = state.shared_gfx_state->renderer_vertex_shaders;
			renderer_fragment_shaders = state.shared_gfx_state->renderer_fragment_shaders;
		}
	};


	PipelineThingy ProcessRoUniforms(const GraphicsStateInterface& state, UboManager& ubo_manager,StandardBindings& binders)
	{
		auto& mesh_vtx            = state.renderer_vertex_shaders[VNormalMesh];
		auto& skinned_mesh_vtx    = state.renderer_vertex_shaders[VSkinnedMesh];
		auto& skinned_mesh_render = *state.skinned_mesh_render;

		//auto& binders = *binder;
		PipelineThingy the_interface{};
		the_interface.SetRef(ubo_manager);

		the_interface.BindShader(ShaderStage::Vertex, mesh_vtx);
		the_interface.reserve(state.mesh_render->size() + state.skinned_mesh_render->size());
		binders.Bind(the_interface);
		{
			//auto range_opt = state.range;
			//if (!range_opt)
			//	range_opt = GraphicsSystem::RenderRange{ CameraData{},0,state.inst_ro->size() };
			
			//auto& inst_draw_range = *range_opt;
			std::visit([&](auto& inst_draw_range) {
				for (auto itr = state.inst_ro->data() + inst_draw_range.inst_mesh_render_begin,
					end = state.inst_ro->data() + inst_draw_range.inst_mesh_render_end;
					itr != end; ++itr
					)
				{
					auto& dc = *itr;
					auto& mat_inst = *dc.material_instance;
					if (mat_inst.material && !binders.Skip(the_interface,dc))
					{
						binders.Bind(the_interface, dc);
						the_interface.BindMeshBuffers(dc);
						the_interface.BindAttrib(4,state.shared_state->inst_mesh_render_buffer.buffer(),0);
						the_interface.FinalizeDrawCall(dc, dc.num_instances, dc.instanced_index);
					}
				}
			},state.range);
		}

		{
			const vector<const AnimatedRenderObject*>& draw_calls = skinned_mesh_render;
			the_interface.BindShader(ShaderStage::Vertex, skinned_mesh_vtx);
			binders.Bind(the_interface);
			for (auto& ptr_dc : draw_calls)
			{
				auto& dc = *ptr_dc;
				auto& mat_inst = *dc.material_instance;
				if (mat_inst.material && dc.layer_mask&state.mask || !binders.Skip(the_interface, dc))
				{
					binders.Bind(the_interface, dc);
					if(!the_interface.BindMeshBuffers(dc))
						continue;
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
		_pimpl = std::make_shared<PImpl>();
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
			auto thread = CreateRenderThread();
			thread->Init(this);
			_render_threads.emplace_back(std::move(thread));
		}
		_pre_render_complete = device.createSemaphoreUnique(vk::SemaphoreCreateInfo{});
		_post_render_complete = device.createSemaphoreUnique(vk::SemaphoreCreateInfo{});
		_convoluter.pipeline_manager(*_pipeline_manager);
		_convoluter.Init(
			Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VPBRConvolute],
			Core::GetSystem<GraphicsSystem>().renderer_fragment_shaders[FPBRConvolute],
			Core::GetSystem<GraphicsSystem>().renderer_geometry_shaders[GSinglePassCube]
		);
		_particle_renderer.InitConfig();
		_font_renderer.InitConfig();
		_canvas_renderer.InitConfig();
	}
	void FrameRenderer::SetPipelineManager(PipelineManager& manager)
	{
		_pipeline_manager = &manager;
	}
	void FrameRenderer::PreRenderGraphicsStates(const PreRenderData& state, uint32_t frame_index)
	{
		auto& lights = *state.shared_gfx_state->lights;
		const size_t num_conv_states = 1;
		const size_t num_instanced_buffer_state = 1;
		const size_t num_color_pick_states = 1;
		auto total_pre_states = lights.size() + num_conv_states+ num_instanced_buffer_state + num_color_pick_states;
		GrowStates(_pre_states, total_pre_states);
		for (auto& pre_state : _pre_states)
		{
			pre_state.Reset();
		}
		size_t curr_state = 0;


		std::optional<vk::Semaphore> copy_semaphore{};
		{

			auto copy_state_ind = curr_state++;
			auto& copy_state = _pre_states[copy_state_ind];

			auto& instanced_data = *state.inst_mesh_buffer;
			
			copy_semaphore = *copy_state.signal.render_finished;

			auto cmd_buffer = copy_state.CommandBuffer();
			cmd_buffer.begin(vk::CommandBufferBeginInfo{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
			if (instanced_data.size())
			{
				state.shared_gfx_state->inst_mesh_render_buffer.resize(hlp::buffer_size(instanced_data));
				state.shared_gfx_state->inst_mesh_render_buffer.update<const InstancedData>(vk::DeviceSize{ 0 }, instanced_data, cmd_buffer);
				for (auto& [buffer, data,offset] : state.shared_gfx_state->update_instructions)
				{
					if (data.size())
					{
						buffer->update(offset,s_cast<uint32_t>(data.size()),cmd_buffer,std::data(data));
					}
				}
			}
			if (state.shared_gfx_state->particle_data && state.shared_gfx_state->particle_data->size())
			{
				auto& particle_data = *state.shared_gfx_state->particle_data;
				auto& buffer = state.shared_gfx_state->particle_buffer;
				buffer.resize(hlp::buffer_size(particle_data));
				buffer.update<const ParticleObj>(0, particle_data, cmd_buffer);
			}
			if (state.shared_gfx_state->fonts_data && state.shared_gfx_state->fonts_data->size())
			{
				auto& font_data = *state.shared_gfx_state->fonts_data;
				auto& buffer = state.shared_gfx_state->font_buffer;
				
				buffer.resize(font_data.size());
				for (unsigned i = 0; i < font_data.size(); ++i)
				{
					auto& b = buffer[i];
					b.resize(hlp::buffer_size(font_data[i].coords));
					b.update<const FontPoint>(0, font_data[i].coords, cmd_buffer);
				}
				
			}

			cmd_buffer.end();
			//copy_state.FlagRendered();//Don't flag, we want to submit this separately.

			vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eTransfer };
			vk::SubmitInfo submit_info
			{
				0
				,nullptr
				,waitStages
				,1,&copy_state.CommandBuffer()
				,1,&*copy_semaphore
			};

			auto queue = View().GraphicsQueue();
			queue.submit(submit_info, vk::Fence{}, vk::DispatchLoaderDefault{});
		}

		{
			auto& color_picker = _pimpl->color_picker;
			auto& requests = _pimpl->color_pick_requests;
			auto& shared_gs = *state.shared_gfx_state;
			auto& rs = _pre_states[curr_state++];
			auto cmd_buffer = rs.CommandBuffer();
			cmd_buffer.begin(vk::CommandBufferBeginInfo{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
			color_picker.PreRender(requests, shared_gs, state.inst_mesh_buffer->size(), cmd_buffer);
			color_picker.Render(requests, shared_gs, rs);
		}
		//Do post pass here
		//Canvas pass
		for (auto light_idx : state.active_lights)
		{
			auto& rs = _pre_states[curr_state++];
			PreRenderShadow(light_idx, state, rs, frame_index);
		}
		//TODO: Submit the command buffers

		vector<vk::CommandBuffer> buffers{};


		auto& cameras = *state.cameras;
		if (cameras.size()==0)
		{
			auto& convolute_state = _pre_states[curr_state];
			_convoluter.pipeline_manager(*_pipeline_manager);
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
			auto cmd_buffer = convolute_state.CommandBuffer();
			cmd_buffer.begin(vk::CommandBufferBeginInfo
				{
					vk::CommandBufferUsageFlagBits::eOneTimeSubmit
				}
			);
			_convoluter.ProcessQueue(cmd_buffer);
			cmd_buffer.end();

		}

		for (auto& pre_state : _pre_states)
		{
			if (pre_state.has_commands)
				buffers.emplace_back(pre_state.CommandBuffer());
		}

		vector<vk::Semaphore> arr_ready_sem{ *_pre_render_complete };
		vector<vk::Semaphore> arr_wait_sem {};
		if (copy_semaphore)
			arr_wait_sem.emplace_back(*copy_semaphore);
		vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eAllCommands };


		vk::SubmitInfo submit_info
		{
			hlp::arr_count(arr_wait_sem) ,std::data(arr_wait_sem)
			,waitStages
			,hlp::arr_count(buffers),std::data(buffers)
			,hlp::arr_count(arr_ready_sem) ,std::data(arr_ready_sem)
		};

		auto queue = View().GraphicsQueue();
		queue.submit(submit_info, vk::Fence{}, vk::DispatchLoaderDefault{});
	}
	VulkanView& View();
//#pragma optimize("",off)
	void RenderPipelineThingy(
		[[maybe_unused]] const SharedGraphicsState& shared_state,
		PipelineThingy&     the_interface      ,
		PipelineManager&    pipeline_manager   ,
		vk::CommandBuffer   cmd_buffer         , 
		const vector<vec4>& clear_colors       ,
		vk::Framebuffer     frame_buffer       ,
		RenderPassObj       rp                 ,
		bool                has_depth_stencil  ,
		vk::Rect2D          render_area        ,
		vk::Rect2D          viewport           ,
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
			*rp, frame_buffer,
			render_area,hlp::arr_count(clear_value),std::data(clear_value)
		};


		cmd_buffer.beginRenderPass(rpbi, vk::SubpassContents::eInline);

		auto& processed_ro = the_interface.DrawCalls();
		shared_ptr<const pipeline_config> prev_config{};
		for (auto& p_ro : processed_ro)
		{
			bool is_mesh_renderer = p_ro.vertex_shader == Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VNormalMesh];
			auto& obj = p_ro.Object();
			if (p_ro.rebind_shaders||prev_config!=obj.config)
			{
				prev_config = obj.config;
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

				if (is_mesh_renderer)
					config.buffer_descriptions.emplace_back(
						buffer_desc
						{
							buffer_desc::binding_info{ std::nullopt,sizeof(mat4) * 2,VertexRate::eInstance},
							{buffer_desc::attribute_info{AttribFormat::eMat4,4,0,true},
							 buffer_desc::attribute_info{AttribFormat::eMat4,8,sizeof(mat4),true}
							 }
						}
				);
				auto& pipeline = pipeline_manager.GetPipeline(config, shaders, frame_index, rp, has_depth_stencil);
				pipeline.Bind(cmd_buffer, View());
				SetViewport(cmd_buffer, *config.viewport_offset, *config.viewport_size);
				prev_pipeline = &pipeline;
			}
			auto& pipeline = *prev_pipeline;
			//TODO Grab everything and render them
			//auto& mat = obj.material_instance.material.as<VulkanMaterial>();
			//auto& mesh = obj.mesh.as<VulkanMesh>();
			{
				uint32_t set = 0;
				for (auto& ods : p_ro.descriptor_sets)
				{
					if (ods)
					{
						auto& ds = *ods;
						cmd_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *pipeline.pipelinelayout, set, ds, {});
					}
					++set;
				}
			}

			//auto& renderer_req = *obj.renderer_req;

			for (auto&& [location, attrib] : p_ro.attrib_buffers)
			{
				cmd_buffer.bindVertexBuffers(*pipeline.GetBinding(location), attrib.buffer, vk::DeviceSize{ attrib.offset }, vk::DispatchLoaderDefault{});
			}
			auto& oidx = p_ro.index_buffer;
			if (oidx)
			{
				cmd_buffer.bindIndexBuffer(oidx->buffer, 0, oidx->index_type, vk::DispatchLoaderDefault{});
				cmd_buffer.drawIndexed(s_cast<uint32_t>(p_ro.num_vertices), static_cast<uint32_t>(p_ro.num_instances), 0, 0, static_cast<uint32_t>(p_ro.inst_offset), vk::DispatchLoaderDefault{});
			}
			else
			{
				cmd_buffer.draw(s_cast<uint32_t>(p_ro.num_vertices), s_cast<uint32_t>(p_ro.num_instances), 0, s_cast<uint32_t>(p_ro.inst_offset), vk::DispatchLoaderDefault{});
			}
		}
	}

	void FrameRenderer::PreRenderShadow(size_t light_index, const PreRenderData& state, RenderStateV2& rs, uint32_t frame_index)
	{
		const LightData& light = state.shared_gfx_state->Lights()[light_index];
		auto cam = CameraData{ GenericHandle {}, LayerMask{0xFFFFFFFF }, light.v, light.p};
		ShadowBinding shadow_binding;
		shadow_binding.for_each_binder<has_setstate>(
			[](auto& binder, const CameraData& cam, const vector<SkeletonTransforms>& skel)
			{
				binder.SetState(cam, skel);
			},
			cam,
				*state.skeleton_transforms);
		GraphicsStateInterface gsi = { state };
		gsi.range = (*state.shadow_ranges)[light_index];
		auto the_interface = vkn::ProcessRoUniforms(gsi, rs.ubo_manager, shadow_binding);
		the_interface.GenerateDS(rs.dpools);

		//auto& swapchain = view.Swapchain();
		auto dispatcher = vk::DispatchLoaderDefault{};
		vk::CommandBuffer cmd_buffer = rs.CommandBuffer();
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
		auto  rp = rt.GetRenderPass ();
		rt.PrepareDraw(cmd_buffer);
		vector<vec4> clear_colors
		{
			vec4{1}
		};
		if (the_interface.DrawCalls().size())
			rs.FlagRendered();
		RenderPipelineThingy(*state.shared_gfx_state,the_interface, GetPipelineManager(), cmd_buffer, clear_colors, fb, rp, true, render_area,render_area,frame_index);

		rs.ubo_manager.UpdateAllBuffers();
		cmd_buffer.endRenderPass();
		cmd_buffer.end();

	}

	void FrameRenderer::PostRenderCanvas(size_t& canvas_count,RscHandle<RenderTarget> rr, const vector<UIRenderObject>& canvas_data, const PostRenderData& state, RenderStateV2& rs, uint32_t frame_index)
	{
		auto& rt = rr.as<VknRenderTarget>();
		//auto& swapchain = view.Swapchain();
		auto dispatcher = vk::DispatchLoaderDefault{};
		vk::CommandBuffer cmd_buffer = rs.CommandBuffer();
		vk::CommandBufferBeginInfo begin_info{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit,nullptr };
		GraphicsStateInterface gsi = { state };

		//Generate this pipeline thingy
		PipelineThingy the_interface{};
		
		the_interface.SetRef(rs.ubo_manager);
		_canvas_renderer.DrawCanvas(canvas_count,the_interface, state, rs, canvas_data);
		the_interface.GenerateDS(rs.dpools);

		cmd_buffer.begin(begin_info, dispatcher);
		//auto sz = light.light_map->DepthAttachment().buffer->Size();
		//auto sz = wewew.DepthAttachment().buffer->Size();
		
		vk::Framebuffer fb = rt.Buffer();
		auto  rp = rt.GetRenderPass(false, false);

		rt.PrepareDraw(cmd_buffer);
		
		auto sz = rt.Size();
		vk::Rect2D render_area
		{
			vk::Offset2D{0,0},
			vk::Extent2D{s_cast<uint32_t>(rt.size.x),s_cast<uint32_t>(rt.size.y)}
		};

		vector<vec4> clear_colors
		{
			vec4{1},
			vec4{0.f}
		};
		if (the_interface.DrawCalls().size())
			rs.FlagRendered();
		RenderPipelineThingy(*state.shared_gfx_state, the_interface, GetPipelineManager(), cmd_buffer, clear_colors, fb, rp, true, render_area, render_area, frame_index);

		rs.ubo_manager.UpdateAllBuffers();
		cmd_buffer.endRenderPass();
		cmd_buffer.end();
	}
//#pragma optimize("",off)
	void FrameRenderer::RenderGraphicsStates(const vector<GraphicsState>& gfx_states, uint32_t frame_index)
	{
		_current_frame_index = frame_index;
		//Update all the resources that need to be updated.
		auto& curr_frame = *this;
		GrowStates(_states, gfx_states.size());
		size_t num_concurrent = curr_frame._render_threads.size();
		auto& pri_buffer = curr_frame._pri_buffer;
		auto& transition_buffer = curr_frame._transition_buffer;
		auto queue = View().GraphicsQueue();
		auto& swapchain = View().Swapchain();
		for (auto& state : curr_frame._states)
		{
			state.Reset();
		}
		ivec2 max_size{};
		for (auto& gfx_state : gfx_states)
		{
			auto sz = gfx_state.camera.render_target->Size();
			max_size.x = std::max(sz.x, max_size.x);
			max_size.y = std::max(sz.y, max_size.y);
		}
		if (max_size.x > _gbuffer_size.x || max_size.y > _gbuffer_size.y)
		{
			for (auto& gbuffer : _gbuffers)
			{
				if(gbuffer.Init(max_size))
					_pimpl->deferred_buffers.clear();
			}
		}
		for (auto i = gfx_states.size(); i-- > 0;)
		{
			if (Core::GetSystem<GraphicsSystem>().is_deferred())
			{
				auto& camera = gfx_states[i].camera;
				auto& deferred_pass=_states[i].deferred_pass;
				deferred_pass.fullscreen_quad_vert = Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VertexShaders::VFsq];
				deferred_pass.deferred_post_frag[EGBufferType::map(GBufferType::eMetallic)] = Core::GetSystem<GraphicsSystem>().renderer_fragment_shaders[FragmentShaders::FDeferredPost];
				deferred_pass.deferred_post_frag[EGBufferType::map(GBufferType::eSpecular)]= Core::GetSystem<GraphicsSystem>().renderer_fragment_shaders[FragmentShaders::FDeferredPostSpecular];
				deferred_pass.deferred_post_ambient = Core::GetSystem<GraphicsSystem>().renderer_fragment_shaders[FragmentShaders::FDeferredPostAmbient];
				auto& rt = camera.render_target.as<VknRenderTarget>();
				auto& deferred_buffers = _pimpl->deferred_buffers;
				auto color_buffer = rt.GetColorBuffer();
				auto hdr_itr = deferred_buffers.find(color_buffer);
				if (hdr_itr != deferred_buffers.end())
				{
					deferred_pass.hdr_buffer = hdr_itr->second;
				}
				deferred_pass.Init(rt,_gbuffers);
				deferred_buffers[color_buffer] = deferred_pass.hdr_buffer;
			}
		}
		bool rendered = false;
		{
			;
			for (size_t i = 0; i  < gfx_states.size(); i += num_concurrent)
			{
				auto curr_concurrent = std::min(num_concurrent,gfx_states.size()-i);
				//Spawn/Assign to the threads
				for (size_t j = 0; j < curr_concurrent; ++j) {
					auto& state = gfx_states[i + j];
					auto& rs = _states[i + j];
					_render_threads[j]->Render(state, rs);
					rendered = true;
					//TODO submit command buffer here and signal the framebuffer's stuff.
					//TODO create two renderpasses, detect when a framebuffer is used for the first time, use clearing renderpass for the first and non-clearing for the second onwards.
					//OR sort the gfx states so that we process all the gfx_states that target the same render target within the same command buffer/render pass.
					//RenderGraphicsState(state, curr_frame.states[j]);//We may be able to multi thread this
				}
			}

		}
		//Join with all the threads
		for (size_t j = 0; j < _render_threads.size(); ++j) {
			auto& thread = _render_threads[j];
			thread->Join();
		}
		pri_buffer->reset({}, vk::DispatchLoaderDefault{});
		vector<vk::CommandBuffer> buffers{};
		for (auto& state : curr_frame._states)
		{
			if (state.has_commands)
				buffers.emplace_back(state.CommandBuffer());
		}
		vk::CommandBufferBeginInfo begin_info{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit };
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

		//if (!rendered)
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

		if (!rendered)
			buffers.emplace_back(*pri_buffer);
		auto& current_signal = View().CurrPresentationSignals();

		vector<vk::Semaphore> waitSemaphores{ *current_signal.image_available, *_pre_render_complete };
		vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eAllCommands,vk::PipelineStageFlagBits::eAllCommands };
		vk::Semaphore readySemaphores =*_states[0].signal.render_finished;
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
		auto inflight_fence = *_states[0].signal.inflight_fence();



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
		View().Swapchain().m_graphics.images[View().vulkan().rv] = RscHandle<VknRenderTarget>()->GetColorBuffer().as<VknTexture>().Image();
	}
//#pragma optimize ("",off)
	void FrameRenderer::PostRenderGraphicsStates(const PostRenderData& state, uint32_t frame_index)
	{
		//auto& lights = *state.shared_gfx_state->lights;

		auto& canvas = *state.shared_gfx_state->ui_canvas;
		size_t num_conv_states = 1;
		size_t num_instanced_buffer_state = 1;
		auto total_post_states = canvas.size() + num_conv_states + num_instanced_buffer_state;
		GrowStates(_post_states, total_post_states);
		for (auto& pos_state : _post_states)
		{
			pos_state.Reset();
		}
		size_t curr_state = 0;
		std::optional<vk::Semaphore> copy_semaphore{};
		{

			auto copy_state_ind = curr_state++;
			auto& copy_state = _post_states[copy_state_ind];

			copy_semaphore = *copy_state.signal.render_finished;

			auto cmd_buffer = copy_state.CommandBuffer();
			cmd_buffer.begin(vk::CommandBufferBeginInfo{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit });

			if (state.shared_gfx_state->ui_canvas && state.shared_gfx_state->ui_canvas->size())
			{
				//auto& canvas_data = canvas;
				auto& pos_buffer = state.shared_gfx_state->ui_text_buffer_pos;
				auto& uv_buffer = state.shared_gfx_state->ui_text_buffer_uv;
				//auto& buffer = state.shared_gfx_state->ui_text_buffer;
				auto& doto = *state.shared_gfx_state->ui_text_data;
				auto& t_size = state.shared_gfx_state->total_num_of_text;
				//auto& canvas_range_data = *state.shared_gfx_state->ui_canvas_range;

				pos_buffer.resize(t_size);
				uv_buffer.resize(t_size);

				unsigned i = 0;
				//size_t offset_size = 0;
				//size_t range = 0;
				for (auto& elem : doto)
				{
					elem;
					auto& b = pos_buffer[i];
					b.resize(hlp::buffer_size(doto[i].pos));
					b.update<const vec2>(0, doto[i].pos, cmd_buffer);
					auto& b1 = uv_buffer[i];
					b1.resize(hlp::buffer_size(doto[i].uv));
					b1.update<const vec2>(0, doto[i].uv, cmd_buffer);

					//range = hlp::buffer_size(doto[i].pos);

					//canvas_range_data.emplace_back(CanvasRenderRange{offset_size,offset_size + range});
					//offset_size += range;

					++i;
				}
				
			}
			cmd_buffer.end();
			//copy_state.FlagRendered();//Don't flag, we want to submit this separately.

			vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eTransfer };
			vk::SubmitInfo submit_info
			{
				1
				,&*_states[0].signal.render_finished
				,waitStages
				,1,&copy_state.CommandBuffer()
				,1,&*copy_semaphore
			};

			auto queue = View().GraphicsQueue();
			queue.submit(submit_info, vk::Fence{}, vk::DispatchLoaderDefault{});
		}

		//Do post pass here
		//Canvas pass
		size_t i = 0;
		for (auto& elem : canvas)
		{
			auto& rs = _post_states[curr_state++];
			//if(elem.render_target) //Default render target is null. Don't ignore it.
			PostRenderCanvas(i,elem.render_target, elem.ui_ro, state, rs, frame_index);
		}
		//TODO: Submit the command buffers

		vector<vk::CommandBuffer> buffers{};

		for (auto& post_state : _post_states)
		{
			if (post_state.has_commands)
				buffers.emplace_back(post_state.CommandBuffer());
		}

		vector<vk::Semaphore> arr_ready_sem{ *_post_render_complete };
		vector<vk::Semaphore> arr_wait_sem{};
		if (copy_semaphore)
			arr_wait_sem.emplace_back(*copy_semaphore);
		vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eAllCommands };


		vk::SubmitInfo submit_info
		{
			hlp::arr_count(arr_wait_sem) ,std::data(arr_wait_sem)
			,waitStages
			,hlp::arr_count(buffers),std::data(buffers)
			,hlp::arr_count(arr_ready_sem) ,std::data(arr_ready_sem)
		};

		auto queue = View().GraphicsQueue();
		queue.submit(submit_info, vk::Fence{}, vk::DispatchLoaderDefault{});
	}
	PresentationSignals& FrameRenderer::GetMainSignal()
	{
		return _states[0].signal;
	}
	void FrameRenderer::GrowStates(vector<RenderStateV2>& states, size_t new_min_size)
	{
		auto device = *View().Device();
		if (new_min_size > states.size())
		{
			auto diff = s_cast<uint32_t>(new_min_size - states.size());
			for (auto i = diff; i-- > 0;)
			{
				auto cmd_pool = View().vulkan().CreateGfxCommandPool();
				auto&& buffers = device.allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo{ *cmd_pool,vk::CommandBufferLevel::ePrimary, 1}, vk::DispatchLoaderDefault{});
				auto& buffer = buffers[0];
				states.emplace_back(RenderStateV2{std::move(cmd_pool), std::move(buffer),UboManager{View()},PresentationSignals{},DescriptorsManager{View()},CubemapRenderer{} }).signal.Init(View());
				//_state_cmd_buffers.emplace_back(std::move(buffer));
			}
		}
	}
	//Assumes that you're in the middle of rendering other stuff, i.e. command buffer's renderpass has been set
	//and command buffer hasn't ended
	void FrameRenderer::RenderDebugStuff(const GraphicsState& state, RenderStateV2& rs,ivec2 vp_pos, ivec2 vp_size)
	{
		auto dispatcher = vk::DispatchLoaderDefault{};
		vk::CommandBuffer cmd_buffer = rs.CommandBuffer();
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
			DescriptorUpdateData dud;
			auto ds_layout = itr->second;
			auto allocated =rs.dpools.Allocate(hash_table<vk::DescriptorSetLayout, std::pair<vk::DescriptorType, uint32_t>>{ {ds_layout, {vk::DescriptorType::eUniformBuffer,2}}});
			auto aitr = allocated.find(ds_layout);
			if (aitr != allocated.end())
			{
				auto&& [view_buffer, vb_offset] = rs.ubo_manager.Add(state.camera.view_matrix);
				auto&& [proj_buffer, pb_offset] = rs.ubo_manager.Add(mat4{ 1,0,0,0,   0,1,0,0,   0,0,0.5f,0.5f, 0,0,0,1 }*state.camera.projection_matrix);
				auto ds = aitr->second.GetNext();
				UpdateUniformDS(ds, vector<ProcessedRO::BindingInfo>{ 
					ProcessedRO::BindingInfo{
						0,view_buffer,vb_offset,0,sizeof(mat4),itr->second
					},
					ProcessedRO::BindingInfo{ 1,proj_buffer,pb_offset,0,sizeof(mat4),itr->second }
				},dud
				);
				dud.SendUpdates();
				cmd_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *pipeline->pipelinelayout, 0, ds, {});
			}
		}
		
		for (auto& p_dc : state.dbg_render)
		{
			
			auto& dc = *p_dc;
			dc.Bind(cmd_buffer);
			//cmd_buffer.bindVertexBuffers(0,
			//	{
			//		 *mesh.Get(attrib_index::Position).buffer(),//dc.mesh_buffer[DbgBufferType::ePerVtx].find(0)->second.buffer,
			//		dc.mesh_buffer[DbgBufferType::ePerInst].find(1)->second.buffer
			//	},
			//	{
			//		0,0
			//	}
			//	);
			//cmd_buffer.bindIndexBuffer(dc.index_buffer.buffer, 0, vk::IndexType::eUint16);
			//cmd_buffer.drawIndexed(mesh.IndexCount(), dc.nu, 0, 0, 0);
			dc.Draw(cmd_buffer);
			
		}
		
	}
	vk::RenderPass FrameRenderer::GetRenderPass(const GraphicsState& state, VulkanView&)
	{
		//vk::RenderPass result = view.BasicRenderPass(BasicRenderPasses::eRgbaColorDepth);
		//if (state.camera.is_shadow)
		//	result = view.BasicRenderPass(BasicRenderPasses::eDepthOnly);
		return *state.camera.render_target.as<VknRenderTarget>().GetRenderPass(state.clear_render_target && state.camera.clear_data.index() != meta::IndexOf <std::remove_const_t<decltype(state.camera.clear_data)>,DontClear>::value);
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
	void FrameRenderer::ColorPick(vector<ColorPickRequest>&& pick_buffer)
	{
		_pimpl->color_pick_requests = std::move(pick_buffer);
	}
	void FrameRenderer::RenderGraphicsState(const GraphicsState& state, RenderStateV2& rs)
	{
		bool is_deferred = Core::GetSystem<GraphicsSystem>().is_deferred();

		auto& view = View();
		//auto& swapchain = view.Swapchain();
		auto& camera = state.camera;
		auto sz = camera.render_target->Size();
		auto [offset, size] = ComputeVulkanViewport(vec2{ sz }, camera.viewport);

		auto dispatcher = vk::DispatchLoaderDefault{};
		vk::CommandBuffer cmd_buffer = rs.CommandBuffer();
		vk::CommandBufferBeginInfo begin_info{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit,nullptr };

		cmd_buffer.begin(begin_info, dispatcher);
		VulkanPipeline* prev_pipeline = nullptr;
		vector<RscHandle<ShaderProgram>> shaders;
		auto& deferred_pass = rs.deferred_pass;
		if (is_deferred)
		{
			deferred_pass.pipeline_manager(GetPipelineManager());
			deferred_pass.frame_index(_current_frame_index);
			dbg::BeginLabel(cmd_buffer, "DrawToGbuffers");
			deferred_pass.DrawToGBuffers(cmd_buffer, state, rs);
			dbg::EndLabel(cmd_buffer);
		}
				

		//Preprocess MeshRender's uniforms
		//TODO make ProcessRoUniforms only render forward pass stuff.
		auto&& the_interface = (is_deferred) ? [](auto& state,auto& rs) {
			//UnlitMaterialBinding binders;
			//binders.for_each_binder<has_setstate>([](auto& binder, const GraphicsState& state) {binder.SetState(state); }, state);
			//return vkn::ProcessRoUniforms(state, rs.ubo_manager, binders);
			PipelineThingy the_interface{};
			the_interface.SetRef(rs.ubo_manager);
			return the_interface;
		}(state,rs) : ProcessRoUniforms(state, rs.ubo_manager);
		
		//the_interface.SetRef(rs.ubo_manager);

		_particle_renderer.DrawParticles(the_interface, state, rs);
		_font_renderer.DrawFont(the_interface,state,rs);


		//if(!is_deferred)
		the_interface.GenerateDS(rs.dpools, false);//*/
		the_interface.SetRef(rs.ubo_manager);
		PipelineThingy deferred_interface_light[EGBufferType::size()]{};
		PipelineThingy deferred_interface_hdr  {};
		if (is_deferred)
		{
			GBufferType types[] = { GBufferType::eMetallic,GBufferType::eSpecular };
			for (auto& type : types)
			{
				const size_t max_lights = 7;
				auto deferred_interface = PipelineThingy{};
				deferred_interface.SetRef(rs.ubo_manager);
				rs.deferred_pass.LightPass(type,deferred_interface, state, rs, std::make_pair(0, 0), true);
				for (size_t i = 0; i < state.active_lights.size(); )
				{
					auto num = std::min(max_lights, state.active_lights.size() - i);
					rs.deferred_pass.LightPass(type,deferred_interface,state, rs, std::make_pair(i, i+num),false);
					i += num;
				}
				deferred_interface.GenerateDS(rs.dpools, false);
				deferred_interface_light[EGBufferType::map(type)].~PipelineThingy();
				new (&deferred_interface_light[EGBufferType::map(type)]) PipelineThingy{ std::move(deferred_interface) };

			}
			deferred_interface_hdr.~PipelineThingy();
			auto& hdr_interface = *(new (&deferred_interface_hdr) PipelineThingy{ rs.deferred_pass.HdrPass(state, rs) });
			hdr_interface.GenerateDS(rs.dpools, false);

		}
		
		//rs.ubo_manager.UpdateAllBuffers();
		std::array<float, 4> a{};

		//auto& cd = std::get<vec4>(state.camera.clear_data);
		//TODO grab the appropriate framebuffer and begin renderpass
		std::optional<color> clear_col;
		std::optional<RscHandle<CubeMap>> sb_cm;

		auto& clear_data = state.camera.clear_data;
		switch (clear_data.index())
		{
		case index_in_variant_v<color, CameraClear>:
			clear_col = std::get<color>(clear_data);
			break;
		case index_in_variant_v<RscHandle<CubeMap>, CameraClear>:
			sb_cm = std::get<RscHandle<CubeMap>>(clear_data);
			break;
		case index_in_variant_v<DontClear, CameraClear>:
			//TODO: set dont clear settings.
			break;
		}


		vk::ClearValue clearColor = clear_col ?
			vk::ClearValue{ vk::ClearColorValue{ r_cast<const std::array<float,4>&>(clear_col) } }
			:
			vk::ClearValue{ vk::ClearColorValue{ std::array < float, 4>{0.f,0.f,0.f,0.f} } };
		vk::ClearValue v[]{
			clearColor,
			vk::ClearDepthStencilValue{ 1.0f}
		};
		
		//auto& vvv = state.camera.render_target.as<VknFrameBuffer>();
		
		//auto default_frame_buffer = *swapchain.frame_buffers[swapchain.curr_index];
		auto frame_buffer = GetFrameBuffer(camera, view.CurrFrame());
		TransitionFrameBuffer(camera, cmd_buffer, view);

		

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
		//Begin Clear Pass
		cmd_buffer.beginRenderPass(rpbi, vk::SubpassContents::eInline, dispatcher);
		SetViewport(cmd_buffer, offset, size);
		//SetScissor(cmd_buffer,  offset, size);
		//////////////////Skybox rendering
		if (sb_cm)
		{
			pipeline_config skybox_render_config;
			DescriptorsManager skybox_ds_manager(view);
			skybox_render_config.fill_type = FillType::eFill;
			skybox_render_config.prim_top = PrimitiveTopology::eTriangleList;
			auto config = ConfigWithVP(skybox_render_config,camera,offset,size);
			config.vert_shader = Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VSkyBox];
			config.frag_shader = Core::GetSystem<GraphicsSystem>().renderer_fragment_shaders[FSkyBox];
			config.cull_face = s_cast<uint32_t>(CullFace::eNone);
			config.depth_test = false;
			config.depth_write = false;
			config.render_pass_type = BasicRenderPasses::eRgbaColorDepth;

			//No idea if this is expensive....if really so I will try shift up to init
			rs.skyboxRenderer.pipeline_manager(*_pipeline_manager);
			rs.skyboxRenderer.Init(
				Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VSkyBox],
				Core::GetSystem<GraphicsSystem>().renderer_fragment_shaders[FSkyBox],
				{},
				&config,
				*camera.CubeMapMesh
			);
			rs.skyboxRenderer.QueueSkyBox(rs.ubo_manager, {}, * sb_cm, camera.projection_matrix* mat4{ mat3{camera.view_matrix} });
			
			/*cmd_buffer.begin(vk::CommandBufferBeginInfo
				{
					vk::CommandBufferUsageFlagBits::eOneTimeSubmit
				});*/
			rs.skyboxRenderer.ProcessQueueWithoutRP(cmd_buffer, offset, size);

		}
		cmd_buffer.endRenderPass();//beginRenderPass(rpbi, vk::SubpassContents::eInline, dispatcher);
		auto& rt = camera.render_target.as<VknRenderTarget>();

		//Draw Deferred stuff
		if (is_deferred)
		{
			TransitionFrameBuffer(camera, cmd_buffer, view);
			//for(auto& deferred_interface : deferred_interfaces)
			//Accumulator
			dbg::BeginLabel(cmd_buffer, "DrawToAccum");
			deferred_pass.DrawToAccum(cmd_buffer, deferred_interface_light, camera, rs);
			dbg::EndLabel(cmd_buffer);
			//HDR
			dbg::BeginLabel(cmd_buffer, "DrawToRenderTarget");
			deferred_pass.DrawToRenderTarget(cmd_buffer, deferred_interface_hdr, camera, rt, rs);
			dbg::EndLabel(cmd_buffer);
		}
		//Subsequent passes shouldn't clear the buffer any more.
		rpbi.renderPass = *View().BasicRenderPass(rt.GetRenderPassType(),false,false);
		rs.FlagRendered();
		
		auto& processed_ro = the_interface.DrawCalls();
		bool still_rendering = (processed_ro.size() > 0) ||  camera.render_target->RenderDebug();
		if (still_rendering)
		{
			TransitionFrameBuffer(camera, cmd_buffer, view);
			cmd_buffer.beginRenderPass(rpbi, vk::SubpassContents::eInline, dispatcher);;
		}

		if (processed_ro.size()>0)
		{
			bool is_particle_renderer = false;
			for (auto& p_ro : processed_ro)
			{
				bool is_mesh_renderer = p_ro.vertex_shader == Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VNormalMesh];
				is_particle_renderer = p_ro.vertex_shader == Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VParticle];
				//auto& obj = p_ro.Object();
				if (!p_ro.config)
					continue;
				if (p_ro.rebind_shaders)
				{
					shaders.resize(0);
					if (p_ro.frag_shader)
						shaders.emplace_back(*p_ro.frag_shader);
					if(p_ro.vertex_shader)
						shaders.emplace_back(*p_ro.vertex_shader);
					if(p_ro.geom_shader)
						shaders.emplace_back(*p_ro.geom_shader);

					auto config = ConfigWithVP(*p_ro.config, camera, offset, size);
					if (is_mesh_renderer)
						config.buffer_descriptions.emplace_back(
							buffer_desc
							{
								buffer_desc::binding_info{ std::nullopt,sizeof(mat4)*2,VertexRate::eInstance},
								{buffer_desc::attribute_info{AttribFormat::eMat4,4,0,true},
								 buffer_desc::attribute_info{AttribFormat::eMat4,8,sizeof(mat4),true}
								 }
							}
						);
					auto& pipeline = GetPipeline(config, shaders);
					pipeline.Bind(cmd_buffer,view);
					SetViewport(cmd_buffer, offset, size);
					prev_pipeline = &pipeline;
				}
				auto& pipeline = *prev_pipeline;
				//TODO Grab everything and render them
				//auto& mat = obj.material_instance.material.as<VulkanMaterial>();
				//auto& mesh = obj.mesh.as<VulkanMesh>();
				{
					uint32_t set = 0;
					for (auto& ods : p_ro.descriptor_sets)
					{
						if (ods)
						{
							auto& ds = *ods;
							cmd_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *pipeline.pipelinelayout, set, ds, {});
						}
						++set;
					}
				}
				//auto& renderer_req = *obj.renderer_req;
			
				for (auto& [location, attrib] : p_ro.attrib_buffers)
				{
					auto opt = pipeline.GetBinding(location);
					if (opt)
						cmd_buffer.bindVertexBuffers(*opt, attrib.buffer, vk::DeviceSize{ attrib.offset }, vk::DispatchLoaderDefault{});
				}
				auto& oidx = p_ro.index_buffer;
				if (oidx)
				{
					cmd_buffer.bindIndexBuffer(oidx->buffer, oidx->offset, oidx->index_type, vk::DispatchLoaderDefault{});
					cmd_buffer.drawIndexed(s_cast<uint32_t>(p_ro.num_vertices), s_cast<uint32_t>(p_ro.num_instances), 0, 0, s_cast<uint32_t>(p_ro.inst_offset), vk::DispatchLoaderDefault{});
				}
				else
				{
					cmd_buffer.draw(s_cast<uint32_t>(p_ro.num_vertices), s_cast<uint32_t>(p_ro.num_instances), 0, s_cast<uint32_t>(p_ro.inst_offset), vk::DispatchLoaderDefault{});
				}
			}
		}
		if (camera.render_target->RenderDebug())
		{
			RenderDebugStuff(state, rs, offset, size);
		}
		if (still_rendering)
		{
			cmd_buffer.endRenderPass();
		}
		rs.ubo_manager.UpdateAllBuffers();
		cmd_buffer.end();
		Track(0);
	}

	FrameRenderer::FrameRenderer(FrameRenderer&&)= default;

	FrameRenderer::~FrameRenderer() {}

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