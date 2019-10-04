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
#include <gfx/Light.h>

#include <gfx/MeshRenderer.h>
#include <anim/SkinnedMeshRenderer.h>

namespace idk::vkn
{
	static vk::RenderPass tmp_rp;
	struct SomeHackyThing
	{
		VulkanPipeline pipeline;
	};
	static SomeHackyThing thing;
	void InitThing(VulkanView& view)
	{
		return;
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
				auto vbuffer = Core::GetResourceManager().Load<ShaderProgram>("/assets/shader/mesh.vert.spv").value();
			config.vert_shader = vbuffer;

		}
		{
			{
				auto vbuffer = Core::GetResourceManager().Load<ShaderProgram>("/assets/shader/flat_color.frag.spv").value();
				config.vert_shader = vbuffer;
			}
		}
		config.prim_top = PrimitiveTopology::eTriangleList;
		config.fill_type = FillType::eFill;
		//config.uniform_layouts.emplace()
		thing.pipeline.Create(config, view);
	}
	void RenderStateV2::Reset() {
		cmd_buffer.reset({});
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
	RscHandle<ShaderProgram> LoadShader(string filename, vector<buffer_desc> desc)
	{
		RscHandle<ShaderProgram> result;
		{
			//TODO figure this out
			//auto actualfile = Core::GetSystem<FileSystem>().GetFile(filename);
			//auto rsc = Core::GetResourceManager().GetFileResources(actualfile);
			//if (!actualfile || !rsc.resources.size())
			{

				//vector<buffer_desc> desc{
				//	BufferDesc(0, 0, AttribFormat::eSVec3, sizeof(vec3), eVertex),
				//	BufferDesc(0, 1, AttribFormat::eSVec3, sizeof(vec3), eVertex),
				//	BufferDesc(0, 2, AttribFormat::eSVec2, sizeof(vec2), eVertex),
				//};
				Core::GetSystem<FileSystem>().Update();
				//actualfile = Core::GetSystem<FileSystem>().GetFile(filename);
				result = *Core::GetResourceManager().Load<ShaderProgram>(filename, false);
				auto& mod =result.as<ShaderModule>();
				if(desc.size())
					mod.AttribDescriptions(std::move(desc));
				//_mesh_renderer_shader_module.as<ShaderModule>().Load(vk::ShaderStageFlagBits::eVertex,std::move(desc), strm.str());
				//_mesh_renderer_shader_module = Core::GetResourceManager().Create<ShaderModule>();
			}
		}
		return result;
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


		{
			//TODO figure this out
			string filename = "/assets/shader/mesh.vert";
			_mesh_renderer_shader_module=LoadShader(filename, {
						//BufferDesc(0, 0, AttribFormat::eSVec3, sizeof(vec3), eVertex),
						//BufferDesc(0, 1, AttribFormat::eSVec3, sizeof(vec3), eVertex),
						//BufferDesc(0, 2, AttribFormat::eSVec2, sizeof(vec2), eVertex),
				});
			//auto actualfile = Core::GetSystem<FileSystem>().GetFile(filename);
			////auto rsc = Core::GetResourceManager().GetFileResources(actualfile);
			////if (!actualfile || !rsc.resources.size())
			//{
			//
			//	vector<buffer_desc> desc{
			//		BufferDesc(0, 0, AttribFormat::eSVec3, sizeof(vec3), eVertex),
			//		BufferDesc(0, 1, AttribFormat::eSVec3, sizeof(vec3), eVertex),
			//		BufferDesc(0, 2, AttribFormat::eSVec2, sizeof(vec2), eVertex),
			//	};
			//	Core::GetSystem<FileSystem>().Update();
			//	//actualfile = Core::GetSystem<FileSystem>().GetFile(filename);
			//	_mesh_renderer_shader_module = *Core::GetResourceManager().Load<ShaderProgram>(actualfile,false);
			//	_mesh_renderer_shader_module.as<ShaderModule>().AttribDescriptions(std::move(desc));
			//	//_mesh_renderer_shader_module.as<ShaderModule>().Load(vk::ShaderStageFlagBits::eVertex,std::move(desc), strm.str());
			//	//_mesh_renderer_shader_module = Core::GetResourceManager().Create<ShaderModule>();
			//}
		}
		{
			//TODO figure this out
			string filename = "/assets/shader/skinned_mesh.vert";
			_skinned_mesh_shader_module = LoadShader(filename, {
						//BufferDesc(0, 0, AttribFormat::eSVec3, sizeof(vec3), eVertex),
						//BufferDesc(0, 1, AttribFormat::eSVec3, sizeof(vec3), eVertex),
						//BufferDesc(0, 2, AttribFormat::eSVec2, sizeof(vec2), eVertex),
				});
			//auto actualfile = Core::GetSystem<FileSystem>().GetFile(filename);
			////auto rsc = Core::GetResourceManager().GetFileResources(actualfile);
			////if (!actualfile || !rsc.resources.size())
			//{
			//
			//	vector<buffer_desc> desc{
			//		BufferDesc(0, 0, AttribFormat::eSVec3, sizeof(vec3), eVertex),
			//		BufferDesc(0, 1, AttribFormat::eSVec3, sizeof(vec3), eVertex),
			//		BufferDesc(0, 2, AttribFormat::eSVec2, sizeof(vec2), eVertex),
			//	};
			//	Core::GetSystem<FileSystem>().Update();
			//	//actualfile = Core::GetSystem<FileSystem>().GetFile(filename);
			//	_mesh_renderer_shader_module = *Core::GetResourceManager().Load<ShaderProgram>(actualfile,false);
			//	_mesh_renderer_shader_module.as<ShaderModule>().AttribDescriptions(std::move(desc));
			//	//_mesh_renderer_shader_module.as<ShaderModule>().Load(vk::ShaderStageFlagBits::eVertex,std::move(desc), strm.str());
			//	//_mesh_renderer_shader_module = Core::GetResourceManager().Create<ShaderModule>();
			//}
		}
		//{
		//	string filename = "/assets/shader/shadow.frag";
		//	//auto actualfile = Core::GetSystem<FileSystem>().GetFile(filename);
		//	//auto rsc = Core::GetResourceManager().GetFileResources(actualfile);
		//	auto& shader_mod = _shadow_shader_module;
		//	//if (!actualfile || !rsc.resources.size())
		//	if(!shader_mod)
		//	{
		//
		//		//vector<buffer_desc> desc{
		//		//	BufferDesc(0, 0, AttribFormat::eSVec3, sizeof(vec3), eVertex)
		//		//};
		//		Core::GetSystem<FileSystem>().Update();
		//		//actualfile = Core::GetSystem<FileSystem>().GetFile(filename);
		//		shader_mod = Core::GetResourceManager().Load<ShaderProgram>(filename,false).value();
		//		//shader_mod.as<ShaderModule>().AttribDescriptions(std::move(desc));
		//		//_mesh_renderer_shader_module.as<ShaderModule>().Load(vk::ShaderStageFlagBits::eVertex,std::move(desc), strm.str());
		//		//_mesh_renderer_shader_module = Core::GetResourceManager().Create<ShaderModule>();
		//
		//	}
		//	//else
		//	//{
		//	//	shader_mod = rsc.resources.front().As<ShaderProgram>();
		//	//}
		//}
		{

			vk::AttachmentDescription colorAttachment
			{
				vk::AttachmentDescriptionFlags{}
				,vk::Format::eB8G8R8A8Unorm
				,vk::SampleCountFlagBits::e1
				,vk::AttachmentLoadOp::eClear
				,vk::AttachmentStoreOp::eStore
				,vk::AttachmentLoadOp::eDontCare
				,vk::AttachmentStoreOp::eDontCare
				,vk::ImageLayout::eUndefined
				,vk::ImageLayout::ePresentSrcKHR
			};
			vk::AttachmentReference colorAttachmentRef
			{
				0
				,vk::ImageLayout::eColorAttachmentOptimal
			};

			vk::SubpassDescription subpass
			{
				vk::SubpassDescriptionFlags{}
				,vk::PipelineBindPoint::eGraphics
				,0,nullptr
				,1,&colorAttachmentRef
			};

			vk::SubpassDependency dependency
			{
				VK_SUBPASS_EXTERNAL//src
				,0U				   //dest
				,vk::PipelineStageFlagBits::eColorAttachmentOutput
				,vk::PipelineStageFlagBits::eColorAttachmentOutput
				,vk::AccessFlags{}
				,vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite
			};
			vk::RenderPassCreateInfo renderPassInfo
			{
				vk::RenderPassCreateFlags{}
				,1,&colorAttachment
				,1,&subpass
				,1,&dependency
			};

			tmp_rp = device.createRenderPass(renderPassInfo);
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
			,1,&readySemaphores
		};


		View().Device()->resetFences(1, &inflight_fence, vk::DispatchLoaderDefault{});
		queue.submit(submit_info, inflight_fence, vk::DispatchLoaderDefault{});
		View().Swapchain().m_graphics.images[frame_index] = RscHandle<VknFrameBuffer>()->GetAttachment(AttachmentType::eColor, 0).as<VknTexture>().Image();
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

	string GetUniformData(const UniformInstance&)
	{
		string data;
		return data;
	}
	
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
		collated_bindings[obj_uni.set].emplace_back(
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
		collated_bindings[obj_uni.set].emplace_back(
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
		PreProcUniform<WTF>(obj_uni,index, val->GetAttachment(AttachmentType::eDepth, 0),collated_layouts, collated_bindings);
	}
	template<typename lol=void>
	void BindBones(const UboInfo& info,const AnimatedRenderObject& aro, const vector<SkeletonTransforms>& bones, UboManager& ubos, FrameRenderer::DsBindingCount & collated_layouts, collated_bindings_t & collated_bindings)
	{
		auto&&[buffer,offset]=ubos.Add(bones[aro.skeleton_index]);
		collated_bindings[info.set].emplace_back(info.binding,buffer,offset,0,info.size);
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


	//Possible Idea: Create a Pipeline object that tracks currently bound descriptor sets
	std::pair<vector<FrameRenderer::ProcessedRO>, FrameRenderer::DsBindingCount> FrameRenderer::ProcessRoUniforms(const GraphicsState& state, UboManager& ubo_manager)
	{

		VertexUniformConfig mesh_config, skinned_mesh_config;
		const CameraData& cam = state.camera;
		auto& vkn_fb = cam.render_target.as<VknFrameBuffer>();
		std::pair<vector<ProcessedRO>, DsBindingCount> result{};
		DsBindingCount& collated_layouts = result.second;


		vector<LightData> tmp_light = *state.lights;
		for (auto& light : tmp_light)
		{
			light.v_pos = cam.view_matrix * vec4{ light.v_pos,1 };
			light.v_dir = (cam.view_matrix * vec4{ light.v_dir,0 }).get_normalized();

		}

		string light_block;
		uint32_t len = s_cast<uint32_t>(tmp_light.size());
		light_block += string{ reinterpret_cast<const char*>(&len),sizeof(len) };
		light_block += string( 16-sizeof(len), '\0');
		light_block += string{ reinterpret_cast<const char*>(tmp_light.data()), hlp::buffer_size(tmp_light) };
		auto msprog = GetMeshRendererShaderModule();

		
		auto& msmod = msprog.as<ShaderModule>();

		mesh_config._shader = msprog;
		mesh_config.SetRef(collated_layouts, ubo_manager);
		skinned_mesh_config._shader = _skinned_mesh_shader_module;
		skinned_mesh_config.SetRef(collated_layouts, ubo_manager);


		auto V = cam.view_matrix;//cam.ProjectionMatrix() * cam.ViewMatrix();
		mat4 pvt_trf = mat4{ 1,0,0,0,
							0,1,0,0,
							0,0,0.5f,0.5f,
							0,0,0,1
		}*cam.projection_matrix;

		auto& pvt_uni = msmod.GetLayout("CameraBlock");
		auto& obj_uni = msmod.GetLayout("ObjectMat4Block");
		mat4 pbr_trf = V.inverse();
		;
		auto mesh_mod = msprog;
		//Force pipeline creation
		vector<RscHandle<ShaderProgram>> shaders;

		hash_table<vk::DescriptorSetLayout, int> set_tracker;
		auto layout_incrementer = [](auto& tracking_table, auto layout, int index, auto& layout_count)
		{
			auto itr = tracking_table.find(layout);
			if (itr == tracking_table.end() || itr->second != index)
			{
				layout_count[layout].second++;
			}
			tracking_table[layout] = index;
		};
		auto bind_incr = [&layout_incrementer](auto& tracking_table, int index)
		{
			return [&layout_incrementer, &tracking_table, index](auto layout, auto& layout_count)
			{
				layout_incrementer(tracking_table, layout, index, layout_count);
			};
		};



		int itr_index = 0;
		mesh_config.RegisterBindingInfo("CameraBlock", pvt_trf        );
		skinned_mesh_config.RegisterBindingInfo("CameraBlock", pvt_trf);




		{
			const vector<const RenderObject*>& draw_calls = state.mesh_render;
			for (auto& ptr_dc : draw_calls)
			{
				auto& dc = *ptr_dc;
				//Force pipeline creation
				shaders.resize(0);

				auto& mat_inst = *dc.material_instance;
				auto& mat = *mat_inst.material;
				;
				auto sprog = mat._shader_program;
				std::optional<RscHandle<ShaderProgram>> mesh_shd, geom_shd, frag_shd;
				mesh_shd = msprog;

				auto* fprog = (cam.is_shadow)?nullptr:&sprog.as<ShaderModule>();
				auto& vprog = mesh_mod.as<ShaderModule>();

				if ((!fprog&&!cam.is_shadow) || !mat_inst.material || !vprog)
					continue;

				shaders.emplace_back(mesh_mod);
				if (fprog)
				{
					shaders.emplace_back(*(frag_shd = sprog));
				}
				//TODO Grab everything and render them
				//Maybe change the config to be a managed resource.
				//Force pipeline creation
				auto config = *dc.config;
				config.render_pass_type = vkn_fb.rp_type;
				GetPipeline(config, shaders);
				//set, bindings
				hash_table < uint32_t, vector<ProcessedRO::BindingInfo>> collated_bindings;
				auto& layouts = sprog.as<ShaderModule>();
				auto& lit_uni = layouts.GetLayout("LightBlock");
				auto& pbr_uni = layouts.GetLayout("PBRBlock");

				//Account for the object and normal transform bindings
				mat4 obj_trf = V * dc.transform;
				mat4 obj_ivt= obj_trf.inverse().transpose();
				vector<mat4> mat4_block{obj_trf,obj_ivt};
				PreProcUniform(obj_uni, mat4_block, collated_layouts, collated_bindings, ubo_manager);
				layout_incrementer(set_tracker, obj_uni.layout, itr_index, collated_layouts);
				if (!cam.is_shadow)
				{


					PreProcUniform(pbr_uni, pbr_trf, collated_layouts, collated_bindings, ubo_manager);
					layout_incrementer(set_tracker, pbr_uni.layout, itr_index, collated_layouts);

					PreProcUniform(lit_uni, light_block, collated_layouts, collated_bindings, ubo_manager);
					layout_incrementer(set_tracker, lit_uni.layout, itr_index, collated_layouts);
				}
				//PreProcUniform(nml_uni, obj_ivt, collated_layouts, collated_bindings,ubo_manager);
				//PreProcUniform(pvt_uni, pvt_trf, collated_layouts, collated_bindings,ubo_manager);
				mesh_config.BindRegistered(collated_bindings, bind_incr(set_tracker, itr_index));
				//layout_incrementer(set_tracker, pvt_uni.layout, itr_index, collated_layouts);


				if (!cam.is_shadow)
				{
					auto& layout = fprog->GetLayout("shadow_maps");
					uint32_t i = 0;
					for (auto& shadow_map : state.active_lights)
					{
						auto& sm_uni = shadow_map->light_map;
						{
							PreProcUniform<int>(layout, i++, sm_uni, collated_layouts, collated_bindings);
							layout_incrementer(set_tracker, layout.layout, itr_index, collated_layouts);
						}
					}
					for (; i < layout.size; ++i)
					{
						PreProcUniform<int>(layout, i, RscHandle<Texture>{}, collated_layouts, collated_bindings);
						layout_incrementer(set_tracker, layout.layout, itr_index, collated_layouts);
					}
				}
				if (fprog)
				{
					//Account for material bindings
					for (auto itr = layouts.InfoBegin(), end = layouts.InfoEnd(); itr != end; ++itr)
					{
						auto& name = itr->first;
						auto& mat_inst = *dc.material_instance;
						auto mat_uni_itr = mat_inst.GetUniform(itr->first);
						if (!mat_uni_itr)
						{
							mat_uni_itr = mat_inst.GetUniform(itr->first + "[0]");
						}
							
						if (mat_uni_itr || mat_inst.IsUniformBlock(itr->first))
						{
							auto& ubo_info = itr->second;
							auto& layout = ubo_info.layout;
							{
								layout_incrementer(set_tracker, layout, itr_index, collated_layouts);
								//collated_layouts[layout].second++;

								switch (ubo_info.type)
								{
								case uniform_layout_t::UniformType::eBuffer:
								{
									auto&& data = dc.material_instance->GetUniformBlock(name);
									auto&& [buffer, offset] = ubo_manager.Add(data);
									collated_bindings[ubo_info.set].emplace_back(
										ProcessedRO::BindingInfo
										{
											ubo_info.binding,
											buffer,
											offset,
											0,
											ubo_info.size,
											layout
										}
									);
									collated_layouts[layout].first = vk::DescriptorType::eUniformBuffer;
								}
								break;
								case uniform_layout_t::UniformType::eSampler:
								{
									//for (auto i = ubo_info.size; i-- > 0;)
									{
										auto&& data = dc.material_instance->GetImageBlock(name);// +((ubo_info.size > 1) ? "[" + std::to_string(i) + "]" : ""));
										if (data.size())
										{
											uint32_t i = 0;
											for (auto& htex : data)
											{
												auto& texture = htex.as<vkn::VknTexture>();
												collated_bindings[ubo_info.set].emplace_back(
													ProcessedRO::BindingInfo
													{
														ubo_info.binding,
														ProcessedRO::image_t{*texture.imageView,*texture.sampler,vk::ImageLayout::eGeneral},
														0,
														i,
														ubo_info.size,
												layout
													}
												);
												++i;
											}

										}
									}
									//TODO the pairing is wrong, if two bindings in the same set are of different types, this will cause 1 to be overriden.
									collated_layouts[layout].first = vk::DescriptorType::eCombinedImageSampler;
								}
								break;
								}
							}
						}

					}
				}
				itr_index++;
				result.first.emplace_back(ProcessedRO{ &dc,std::move(collated_bindings),dc.config, mesh_shd,geom_shd,frag_shd });
			}
		}

		{
		const vector<const AnimatedRenderObject*>& draw_calls = state.skinned_mesh_render;
		for (auto& ptr_dc : draw_calls)
		{
			std::optional<RscHandle<ShaderProgram>> mesh_shd, geom_shd, frag_shd;
			auto& dc = *ptr_dc;
			//Force pipeline creation
			shaders.resize(0);

			auto& mat_inst = *dc.material_instance;
			auto& mat = *mat_inst.material;
			;
			auto sprog = mat._shader_program;

			auto* fprog = (cam.is_shadow) ? nullptr : &sprog.as<ShaderModule>();
			mesh_shd = _skinned_mesh_shader_module;
			auto& vprog = mesh_shd->as<ShaderModule>();
			if ((!fprog && !cam.is_shadow) || !mat_inst.material || !vprog)
				continue;

			shaders.emplace_back(*mesh_shd);
			if (fprog)
				shaders.emplace_back( *(frag_shd=sprog));
			//TODO Grab everything and render them
			//Maybe change the config to be a managed resource.
			//Force pipeline creation
			auto config = *dc.config;
			config.render_pass_type = vkn_fb.rp_type;
			GetPipeline(config, shaders);
			//set, bindings
			hash_table < uint32_t, vector<ProcessedRO::BindingInfo>> collated_bindings;
			auto& layouts = sprog.as<ShaderModule>();
			auto& lit_uni = layouts.GetLayout("LightBlock");
			auto& pbr_uni = layouts.GetLayout("PBRBlock");

			//Account for the object and normal transform bindings
			mat4 obj_trf = V * dc.transform;
			mat4 obj_ivt = obj_trf.inverse().transpose();
			vector<mat4> mat4_block{ obj_trf,obj_ivt };
			//PreProcUniform(obj_uni, mat4_block, collated_layouts, collated_bindings, ubo_manager);
			skinned_mesh_config.BindOnce(collated_bindings, "ObjectMat4Block", mat4_block, bind_incr(set_tracker, itr_index));
			//layout_incrementer(set_tracker, obj_uni.layout, itr_index, collated_layouts);

			
			skinned_mesh_config.BindOnce(collated_bindings, "BoneMat4Block", state.GetSkeletonTransforms()[dc.skeleton_index].bones_transforms,bind_incr(set_tracker,itr_index));

			if (!cam.is_shadow)
			{


				PreProcUniform(pbr_uni, pbr_trf, collated_layouts, collated_bindings, ubo_manager);
				layout_incrementer(set_tracker, pbr_uni.layout, itr_index, collated_layouts);

				PreProcUniform(lit_uni, light_block, collated_layouts, collated_bindings, ubo_manager);
				layout_incrementer(set_tracker, lit_uni.layout, itr_index, collated_layouts);
			}
			//PreProcUniform(nml_uni, obj_ivt, collated_layouts, collated_bindings,ubo_manager);
			//PreProcUniform(pvt_uni, pvt_trf, collated_layouts, collated_bindings,ubo_manager);
			skinned_mesh_config.BindRegistered(collated_bindings, bind_incr(set_tracker, itr_index));
			//layout_incrementer(set_tracker, pvt_uni.layout, itr_index, collated_layouts);


			if (!cam.is_shadow)
			{
				auto& layout = fprog->GetLayout("shadow_maps");
				uint32_t i = 0;
				for (auto& shadow_map : state.active_lights)
				{
					auto& sm_uni = shadow_map->light_map;
					{
						PreProcUniform<int>(layout, i++, sm_uni, collated_layouts, collated_bindings);
						layout_incrementer(set_tracker, layout.layout, itr_index, collated_layouts);
					}
				}
				for (; i < layout.size; ++i)
				{
					PreProcUniform<int>(layout, i, RscHandle<Texture>{}, collated_layouts, collated_bindings);
					layout_incrementer(set_tracker, layout.layout, itr_index, collated_layouts);
				}
				
			}
			if (fprog)
			{
				//Account for material bindings
				for (auto itr = layouts.InfoBegin(), end = layouts.InfoEnd(); itr != end; ++itr)
				{
					auto& name = itr->first;
					auto mat_uni_itr = dc.material_instance->GetUniform(itr->first);
					if (!mat_uni_itr)
						mat_uni_itr = dc.material_instance->GetUniform(itr->first + "[0]");
					if (mat_uni_itr)
					{
						auto& ubo_info = itr->second;
						auto& layout = ubo_info.layout;
						{
							layout_incrementer(set_tracker, layout, itr_index, collated_layouts);
							//collated_layouts[layout].second++;

							switch (ubo_info.type)
							{
							case uniform_layout_t::UniformType::eBuffer:
							{
								auto&& data = dc.material_instance->GetUniformBlock(name);
								auto&& [buffer, offset] = ubo_manager.Add(data);
								collated_bindings[ubo_info.set].emplace_back(
									ProcessedRO::BindingInfo
									{
										ubo_info.binding,
										buffer,
										offset,
										0,
										ubo_info.size,
											layout
									}
								);
								collated_layouts[layout].first = vk::DescriptorType::eUniformBuffer;
							}
							break;
							case uniform_layout_t::UniformType::eSampler:
							{
								//for (auto i = ubo_info.size; i-- > 0;)
								{
									auto&& data = dc.material_instance->GetImageBlock(name);// +((ubo_info.size > 1) ? "[" + std::to_string(i) + "]" : ""));
									if (data.size())
									{
										uint32_t i = 0;
										for (auto& htex : data)
										{
											auto& texture = htex.as<vkn::VknTexture>();
											collated_bindings[ubo_info.set].emplace_back(
												ProcessedRO::BindingInfo
												{
													ubo_info.binding,
													ProcessedRO::image_t{*texture.imageView,*texture.sampler,vk::ImageLayout::eGeneral},
													0,
													i,
													ubo_info.size,
											layout
												}
											);
											++i;
										}
										uint32_t total = ubo_info.size;
										for (; i < total; ++i)
										{
											collated_bindings[ubo_info.set].emplace_back(
												ProcessedRO::BindingInfo
												{
													ubo_info.binding,
													ProcessedRO::image_t{vk::ImageView{},vk::Sampler{},vk::ImageLayout::eGeneral},
													0,
													i,
													ubo_info.size,
											layout
												}
											);
										}

									}
								}
								//TODO the pairing is wrong, if two bindings in the same set are of different types, this will cause 1 to be overriden.
								collated_layouts[layout].first = vk::DescriptorType::eCombinedImageSampler;
							}
							break;
							}
						}
					}

				}
			}
			itr_index++;
			result.first.emplace_back(ProcessedRO{ &dc,std::move(collated_bindings),dc.config,mesh_shd,geom_shd,frag_shd });
		}
		}



		return result;
	}
	
	struct DSUpdater
	{
		std::forward_list<vk::DescriptorBufferInfo>& buffer_infos;
		std::forward_list<vector<vk::DescriptorImageInfo>>& image_infos;
		const ProcessedRO::BindingInfo& binding;
		const vk::DescriptorSet& dset;
		vk::WriteDescriptorSet operator()(vk::Buffer ubuffer)
		{
			//auto& dset = ds2[i++];
			buffer_infos.emplace_front(
				vk::DescriptorBufferInfo{
				  ubuffer
				, binding.buffer_offset
				, binding.size
				}
			);
			;

			return
				vk::WriteDescriptorSet{
					dset
					,binding.binding
					,binding.arr_index
					,1
					,vk::DescriptorType::eUniformBuffer
					,nullptr
					,& buffer_infos.front()
					,nullptr
			}
			;
		}
		vk::WriteDescriptorSet operator()(ProcessedRO::image_t ubuffer)
		{
			//auto& dset = ds2[i++];
			vector<vk::DescriptorImageInfo> bufferInfo
			{
				vk::DescriptorImageInfo{
				  ubuffer.sampler
				  ,ubuffer.view
				  ,ubuffer.layout
				}
			};
			;
			image_infos.emplace_front(std::move(bufferInfo));
			return
				vk::WriteDescriptorSet{
					dset
					,binding.binding
					,binding.arr_index
					,hlp::arr_count(image_infos.front())
					,vk::DescriptorType::eCombinedImageSampler
					,std::data(image_infos.front())
					,nullptr
					,nullptr
			}
			;
		}
	};

	void UpdateUniformDS(
		vk::Device& device,
		vk::DescriptorSet& dset,
		vector<ProcessedRO::BindingInfo> bindings
	)
	{
		std::forward_list<vk::DescriptorBufferInfo> buffer_infos;
		std::forward_list<vector<vk::DescriptorImageInfo>> image_infos;
		vector<vk::WriteDescriptorSet> descriptorWrite;
		//TODO: Handle Other DSes as well
		for (auto& binding : bindings)
		{
			DSUpdater updater{buffer_infos,image_infos,binding,dset };
			descriptorWrite.emplace_back(std::visit(updater, binding.ubuffer));
		}
		device.updateDescriptorSets(descriptorWrite, nullptr, vk::DispatchLoaderDefault{});
	}
	vk::Framebuffer GetFrameBuffer(const CameraData& camera_data, uint32_t)
	{
		//TODO Actually get the framebuffer from camera_data
		//auto& e = camera_data.render_target.as<VknFrameBuffer>();
		return camera_data.render_target.as<VknFrameBuffer>().Buffer();
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
		return state.camera.render_target.as<VknFrameBuffer>().GetRenderPass();
	}


	void TransitionFrameBuffer(const CameraData& camera, vk::CommandBuffer cmd_buffer, VulkanView& )
	{
		auto& vkn_fb = camera.render_target.as<VknFrameBuffer>();
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
		auto&& [processed_ro, layout_count] = ProcessRoUniforms(state, rs.ubo_manager);
		auto alloced_dsets = rs.dpools.Allocate(layout_count);


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
		
		auto sz = view.GetWindowsInfo().size;
		vk::Rect2D render_area
		{
			vk::Offset2D{},vk::Extent2D
			{
				s_cast<uint32_t>(sz.x),s_cast<uint32_t>(sz.y)
			}
		};
		auto& camera = state.camera;
		//auto default_frame_buffer = *swapchain.frame_buffers[swapchain.curr_index];
		auto& vkn_fb = camera.render_target.as<VknFrameBuffer>();
		auto frame_buffer = GetFrameBuffer(camera, view.CurrFrame());
		TransitionFrameBuffer(camera, cmd_buffer, view);
		vk::RenderPassBeginInfo rpbi
		{
			GetRenderPass(state,view), frame_buffer,
			render_area,hlp::arr_count(v),std::data(v)
		};


		cmd_buffer.beginRenderPass(rpbi, vk::SubpassContents::eInline, dispatcher);


		rs.FlagRendered();
		for (auto& p_ro : processed_ro)
		{
			auto& obj = p_ro.Object();
			shaders.resize(0);
			//shaders.emplace_back(GetMeshRendererShaderModule());
			auto msprog = *p_ro.vertex_shader;//GetMeshRendererShaderModule();
			auto sprog = (camera.is_shadow) ? _shadow_shader_module : *p_ro.frag_shader;//obj.material_instance->material->_shader_program;
			auto* fprog = (camera.is_shadow) ? nullptr : &sprog.as<ShaderModule>();
			auto opt_gprog = p_ro.geom_shader;
			if ((!camera.is_shadow && !obj.material_instance) || !msprog)
				continue;
			if(fprog)
				shaders.emplace_back(sprog);
			if(opt_gprog)
				shaders.emplace_back(*opt_gprog);
			shaders.emplace_back(msprog);

			//TODO Grab everything and render them
			//Maybe change the config to be a managed resource.
			auto config = *p_ro.config;
			config.render_pass_type = vkn_fb.rp_type;
			auto& pipeline = GetPipeline(config,shaders);
			//auto& mat = obj.material_instance.material.as<VulkanMaterial>();
			if (&pipeline != prev_pipeline)
			{
				pipeline.Bind(cmd_buffer, view);
				prev_pipeline = &pipeline;
			}
			auto& mesh = obj.mesh.as<VulkanMesh>();
			auto& layouts = pipeline.uniform_layouts;
			for (auto& [set_index,binfo] : p_ro.bindings)
			{
				//Get the descriptor set layout for the current set
				//auto layout_itr = layouts.find(set_index);
				//if (layout_itr != layouts.end())
				{
					//Find the allocated pool of descriptor sets that matches the descriptor set layout
					auto layout = binfo.front().GetLayout();
					auto ds_itr = alloced_dsets.find(layout);
					if(ds_itr!=alloced_dsets.end())
					{
						//Get a descriptor set from the allocated pool
						auto ds = ds_itr->second.GetNext();
						//p_ro.dses[set_index] = ds;
						//Update the descriptor set
						UpdateUniformDS(*view.Device(),ds,binfo);
						cmd_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *pipeline.pipelinelayout, set_index, ds, nullptr, dispatcher);
					}
				}
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