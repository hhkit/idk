#include "pch.h"
#include "ColorPickRenderer.h"
#include <vkn/PipelineThingy.h>

#include <vulkan/vulkan.hpp>
#include <vkn/VulkanView.h>
#include <vkn/VknRenderTarget.h>
#include <Core/Core.inl>
#include <res/ResourceManager.inl>	
#include <res/ResourceHandle.inl>	
#include <vkn/MemoryAllocator.h>
#include <vkn/UboManager.inl>

#include <vkn/utils/VknUtil.h>
#include <vkn/PipelineManager.h>
#include <numeric>
#include <renderdoc/renderdoc_app.h>

#include <vkn/DebugUtil.h>
static constexpr bool rd_enabled = false;
RENDERDOC_API_1_1_2*& GetRDocApi();
void RdocFrameCapture()
{
	if constexpr (rd_enabled)
	{
		auto api = GetRDocApi();
		if(api)
			api->StartFrameCapture(NULL, NULL);
	}
}
void RdocEndFrameCapture()
{
	if constexpr (rd_enabled)
	{
		auto api = GetRDocApi();
		if (api)
			api->EndFrameCapture(NULL, NULL);
	}
}
HWND& GetHWND();
namespace idk::vkn
{
	using picking_pixel_t = uint16_t;
	constexpr size_t picking_pixel_size = sizeof(picking_pixel_t);
	VulkanView& View();
	struct RequestInfo
	{
		ColorPickRequest request;
	};

	template<size_t src_size,typename dest_t,size_t dst_size = sizeof(dest_t)>
	dest_t adjust(const byte* start)
	{
		byte ro_index_buffer[std::max(picking_pixel_size, dst_size)] = {};
		std::copy(start, start + src_size, ro_index_buffer);
		auto ro_index = *reinterpret_cast<dest_t*>(ro_index_buffer);
		//TODO: Shift the data according to endianess
		return ro_index;
	}
	struct FrameData
	{
		//Checks if the vulkan buffer is done
		bool Done()
		{
			return View().Device()->getFenceStatus(*fence)==vk::Result::eSuccess;
		}
		//Fulfill the promises
#pragma optimize("",off)
		void Fulfill()
		{
			vk::Device d = *View().Device();
			const byte* memory_range = static_cast<byte*>(d.mapMemory(buffer_memory, 0, mem_chunk_size, vk::MemoryMapFlags{}));
			size_t i = 0;
			for (auto& req : requests)
			{
				auto ro_index = adjust<picking_pixel_size, picking_pixel_t>(memory_range +offset+ picking_pixel_size * i);
				req.select(ro_index); 
				++i;
			}
			d.unmapMemory(buffer_memory);
		}

		vk::UniqueFence fence;
		vk::UniqueBuffer buffer;
		hlp::MemoryAllocator::UniqueAlloc alloc;
		vk::DeviceMemory buffer_memory;
		size_t mem_chunk_size = 0;
		size_t offset;
		vector<ColorPickRequest> requests;
	};
	struct FrameDataHandler
	{
		std::forward_list<FrameData> data;
		vk::UniqueFence MakeFence()
		{
			return View().Device()->createFenceUnique(vk::FenceCreateInfo{ vk::FenceCreateFlags{} });
		}
		FrameData& Add(vector<ColorPickRequest> && requests)
		{
			auto& fd = data.emplace_front(FrameData{ MakeFence(),vk::UniqueBuffer{} });
			fd.requests.reserve(requests.size());
			std::move(requests.begin(), requests.end(), std::back_inserter(fd.requests));
			requests.clear();
			return fd;
		}
		void Check()
		{
			while (!data.empty() && data.front().Done())
			{
				auto& fd = data.front();
				fd.Fulfill();
				data.pop_front();
			}
		}
	};
	struct ColorPickPipeline
	{
		pipeline_config config;
		VulkanPipeline pipeline;
		vector<vk::ShaderModule> pipeline_modules;

		bool DifferentModules(vector<RscHandle<ShaderProgram>>& shaders)
		{
			bool skip = false;
			for (auto& prog : shaders)
			{

				auto& new_mod = prog.as<ShaderModule>();
				//Don't update if any new shader is invalid
				if (new_mod.NeedUpdate())
					new_mod.UpdateCurrent(0);
				skip |= !new_mod.HasCurrent();
			}
			if (skip)
				return false;
			if (shaders.size() != pipeline_modules.size())
				return true;

			return std::transform_reduce(
				shaders.begin(), shaders.end(),
				pipeline_modules.begin(),
				false,
				[](bool lhs, bool rhs)->bool { return lhs | rhs; },
				[](RscHandle<ShaderProgram> prog, vk::ShaderModule mod)
				{
					auto& new_mod = prog.as<ShaderModule>();
					//Don't update if any new shader is invalid
					return  new_mod.HasCurrent() && new_mod.Module() != mod;
				}
			);
		}
#pragma optimize("",off)
		void UpdateModules(vector<RscHandle<ShaderProgram>>& shaders)
		{
			pipeline_modules.resize(shaders.size());
			std::transform(
				shaders.begin(), shaders.end(),
				pipeline_modules.begin(),
				[](RscHandle<ShaderProgram> prog)
				{
					return prog.as<ShaderModule>().Module();
				}
			);
		}
		void ConfigureConfig()
		{
			config.fill_type = FillType::eFill;
			config.cull_face = static_cast<uint32_t>(CullFace::eBack);
			config.depth_test = true;
			config.depth_write = true;
			config.prim_top = PrimitiveTopology::eTriangleList;
			config.stencil_test = false;
		}
		template<typename F>
		void InitPipeline(vector<RscHandle<ShaderProgram>>& shaders,RscHandle<VknFrameBuffer> frame_buffer, F&& config_override)
		{
			if (DifferentModules(shaders))
			{
				ConfigureConfig();
				config_override(config);
				VulkanPipeline::Options options
				{
					{},
					false,
					{
						vk::DynamicState::eViewport,
						vk::DynamicState::eScissor,
					}
				};
				UpdateModules(shaders);
				PipelineDescHelper helper;
				helper.StoreBufferDescOverrides(config);
				helper.UseShaderAttribs(shaders, config);
				pipeline.SetRenderPass(*frame_buffer->GetRenderPass(), true);
				pipeline.Create(config, shaders, View(), options);
			}

		}
	};

	struct ColorPickRenderer::PImpl
	{
		FrameDataHandler handler;
		hlp::vector_buffer id_buffer;
		RscHandle<VknFrameBuffer> frame_buffer;
		hlp::MemoryAllocator allocator;
		ColorPickPipeline mesh_pipeline, skinned_mesh_pipeline;
	};
	ColorPickRenderer::ColorPickRenderer() :_pimpl{ std::make_unique<PImpl>() }
	{
	}
	ColorPickRenderer::~ColorPickRenderer() = default;

	//total_num_insts = total number of instances stored in inst_mesh_render_buffer
	void ColorPickRenderer::PreRender(vector<ColorPickRequest>& requests, const  SharedGraphicsState& shared_gs, size_t total_num_insts, vk::CommandBuffer cmd_buffer)
	{
		if (requests.size() == 0)
			return;
		RdocFrameCapture();
		vector<RscHandle<ShaderProgram>> shaders
		{
			shared_gs.renderer_vertex_shaders[VertexShaders::VNormalMeshPicker],
			shared_gs.renderer_fragment_shaders[FragmentShaders::FPicking]
		};
		vector<RscHandle<ShaderProgram>> skinned_shaders
		{
			shared_gs.renderer_vertex_shaders[VertexShaders::VSkinnedMeshPicker],
			shared_gs.renderer_fragment_shaders[FragmentShaders::FPicking]
		};
		vector<uint32_t> id_buffer(total_num_insts,0);
		ivec2 max_size{};
		for (auto& request : requests)
		{
			auto& render_data = request.data;
			uint32_t id = 1;
			max_size = max(max_size, render_data.camera.render_target.as<VknRenderTarget>().Size());
			//Fill ID buffer
			for (auto itr = (*shared_gs.instanced_ros).data() + render_data.inst_mesh_render_begin, end = (*shared_gs.instanced_ros).data() + render_data.inst_mesh_render_end; itr < end; ++itr)
			{
				auto& ro = *itr;
				auto buffer_start = id_buffer.data() + ro.instanced_index;
				auto buffer_end = buffer_start + ro.num_instances;
				for (auto buffer_itr = buffer_start; buffer_itr != buffer_end; ++buffer_itr)
				{
					*buffer_itr = id++;
				}
			}
		}
		{
			auto& render_data = requests[0].data;
			span range{ shared_gs.instanced_ros->data()+ render_data.inst_skinned_mesh_render_begin,shared_gs.instanced_ros->data() + render_data.inst_skinned_mesh_render_end };
			uint32_t counter = 0;
			for (auto& i_ro: range)
			{
				id_buffer[i_ro.instanced_index] = counter++;
			}
		}
		_pimpl->id_buffer.resize(hlp::buffer_size(id_buffer));
		dbg::BeginLabel(cmd_buffer, "Updating Color Buffer");
		_pimpl->id_buffer.update<uint32_t>(0, id_buffer, cmd_buffer);
		dbg::EndLabel(cmd_buffer);

		auto curr_fb_size = _pimpl->frame_buffer->Size();
		if (curr_fb_size.x < max_size.x || curr_fb_size.y < max_size.y)
		{
			FrameBufferBuilder fbf;
			fbf.Begin("Color Picking",max_size, 1);
			fbf.AddAttachment(
				AttachmentInfo
				{
					LoadOp::eClear,StoreOp::eStore,TextureInternalFormat::R_16_UI,FilterMode::_enum::Nearest
				}
			);
			fbf.SetDepthAttachment(
				AttachmentInfo
				{
					LoadOp::eClear,StoreOp::eStore,TextureInternalFormat::DEPTH_16,FilterMode::_enum::Nearest
				}
			);
			auto& rm = Core::GetResourceManager();
			if (_pimpl->frame_buffer)
				rm.Release(_pimpl->frame_buffer);
			auto handle = rm.GetFactory<FrameBufferFactory>().Create(fbf.End());
			_pimpl->frame_buffer = RscHandle<VknFrameBuffer>{ handle };
		}
		
		_pimpl->mesh_pipeline.InitPipeline(shaders, _pimpl->frame_buffer, [](pipeline_config& config) {
			config.buffer_descriptions.clear();
			config.buffer_descriptions.emplace_back(
				buffer_desc
				{
					buffer_desc::binding_info{ std::nullopt,sizeof(mat4) * 2,VertexRate::eInstance},
					{buffer_desc::attribute_info{AttribFormat::eMat4,4,0,true}
						}
				}
			);
			config.buffer_descriptions.emplace_back(
				buffer_desc
				{
					buffer_desc::binding_info{ std::nullopt,sizeof(uint32_t),VertexRate::eInstance},
					{
						buffer_desc::attribute_info{AttribFormat::eUVec1,1,0,true}
					}
				}
			); });
		_pimpl->skinned_mesh_pipeline.InitPipeline(skinned_shaders,_pimpl->frame_buffer, [](pipeline_config& config) {
			config.buffer_descriptions.clear();
			config.buffer_descriptions.emplace_back(
				buffer_desc
				{
					buffer_desc::binding_info{ std::nullopt,sizeof(mat4) * 2,VertexRate::eInstance},
					{
						buffer_desc::attribute_info{AttribFormat::eMat4,6,0,true}
					}
				}
			);
			config.buffer_descriptions.emplace_back(
				buffer_desc
				{
					buffer_desc::binding_info{ std::nullopt,sizeof(uint32_t),VertexRate::eInstance},
					{
						buffer_desc::attribute_info{AttribFormat::eUVec1,10,0,true}
					}
				}
			); });

	}

	//Main Tasks left:
	//1)Change the render_data stuff to be non-instanced
	//2)Set up the appropriate render_pass
	//3)Set up the appropriate transfer buffer
	void ColorPickRenderer::Render(vector<ColorPickRequest>& requests, const  SharedGraphicsState& shared_gs, RenderStateV2& rs)
	{
		_pimpl->handler.Check();
		if (requests.size() == 0 || !_pimpl->skinned_mesh_pipeline.pipeline.pipeline || !_pimpl->mesh_pipeline.pipeline.pipeline)
			return;
		
		auto& color_pick_frag = shared_gs.renderer_fragment_shaders[FPicking];
		auto& mesh_vtx = shared_gs.renderer_vertex_shaders[VNormalMeshPicker];
		auto& skinned_mesh_vtx = shared_gs.renderer_vertex_shaders[VSkinnedMeshPicker];
		vector<PipelineThingy> render_tasks;

		for (auto& request : requests)
		{
			
			PipelineThingy&  the_interface = render_tasks.emplace_back();
			the_interface.SetRef(rs.ubo_manager);
			auto& render_data = request.data;
			std::array<float, 2> near_far{ render_data.camera.near,render_data.camera.far };
			the_interface.BindShader(ShaderStage::Vertex, mesh_vtx);
			the_interface.BindShader(ShaderStage::Fragment, color_pick_frag);
			the_interface.BindUniformBuffer("NearFarBlock", 0, near_far);
			the_interface.BindUniformBuffer("CameraBlock", 0, render_data.camera.projection_matrix);
			for (auto itr = (*shared_gs.instanced_ros).data()+ render_data.inst_mesh_render_begin, end = (*shared_gs.instanced_ros).data() + render_data.inst_mesh_render_end;itr<end;++itr)
			{
				auto& ro = *itr;
				auto& mat_inst = *ro.material_instance;
				//the_interface.BindUniformBuffer("PerObject", 0, render_data.camera.view_matrix*ro.transform);
				if (mat_inst.material&&(ro.layer_mask & request.data.camera.culling_flags)&&the_interface.BindMeshBuffers(ro))
				{
					the_interface.BindAttrib(4, shared_gs.inst_mesh_render_buffer.buffer(), 0);
					the_interface.BindAttrib(1, _pimpl->id_buffer.buffer(), 0);
					the_interface.FinalizeDrawCall(ro, ro.num_instances, ro.instanced_index);
				}
			}
			the_interface.BindShader(ShaderStage::Vertex, skinned_mesh_vtx);
			std::array<mat4, 2> mtx{ render_data.camera.view_matrix,render_data.camera.projection_matrix };
			the_interface.BindUniformBuffer("CameraBlock", 0, mtx);
			the_interface.BindUniformBuffer("IdBlock", 0, static_cast<uint32_t>(render_data.handles.size())+1);
			for (auto itr = (*shared_gs.instanced_ros).data() + render_data.inst_skinned_mesh_render_begin, end = (*shared_gs.instanced_ros).data() + render_data.inst_skinned_mesh_render_end; itr < end; ++itr)
			{
				auto& ro = *itr;
				auto& dc = *itr;
				auto& mat_inst = *dc.material_instance;
				if (mat_inst.material && dc.layer_mask & request.data.camera.culling_flags)
				{
					if (the_interface.BindMeshBuffers(ro))
					{
						for (auto& [name, ubo] : ro.uniform_buffers)
						{
							the_interface.BindUniformBuffer(name, 0, ubo);
						}
						the_interface.BindAttrib(10, _pimpl->id_buffer.buffer(), 0);
						the_interface.BindAttrib(6, shared_gs.inst_mesh_render_buffer.buffer(), 0);
						the_interface.FinalizeDrawCall(ro, ro.num_instances, ro.instanced_index);
					}
				}
			}
			//TODO skinned stuff
		}
		size_t i = 0;
		
		vk::CommandBuffer cmd_buffer = *rs.cmd_buffer;
		vk::UniqueBuffer uresult_buffer = hlp::CreateBuffer(*View().Device(),requests.size()*sizeof(uint32_t),vk::BufferUsageFlagBits::eTransferDst,View().Dispatcher());
		vk::Buffer result_buffer =*uresult_buffer;
		size_t offset = 0;
		auto& fb = _pimpl->frame_buffer.as<VknFrameBuffer>();
		auto&& rp = fb.GetRenderPass();
		std::array<vk::ClearValue,2> clear_val
		{
			vk::ClearColorValue
			{
				std::array<uint32_t,4>{0,0,0,0}
			},
			vk::ClearDepthStencilValue
			{
				1.0f,0
			}
		};
		auto& fd = _pimpl->handler.Add(std::move(requests));
		fd.buffer = std::move(uresult_buffer);
		auto req_itr = fd.requests.begin();
		fd.alloc = _pimpl->allocator.Allocate(result_buffer, vk::MemoryPropertyFlagBits::eHostVisible);
		fd.mem_chunk_size = fd.alloc->BlockSize();
		fd.buffer_memory = fd.alloc->Memory();
		View().Device()->bindBufferMemory(result_buffer, fd.buffer_memory, 0, View().Dispatcher());
		for (auto& task : render_tasks)
		{
			auto& request = *req_itr;
			auto& data = request.data;
			task.GenerateDS(rs.dpools, (++i)==render_tasks.size());
			auto vp_offset = ivec2{}, vp_size = data.camera.render_target->Size();
			auto rect = hlp::ToRect2D(vp_offset, vp_size);
			vk::RenderPassBeginInfo rpbi
			{
				*rp,fb.GetFramebuffer(),
				rect,
				static_cast<uint32_t>(clear_val.size()),clear_val.data()
			};
			cmd_buffer.beginRenderPass(rpbi,vk::SubpassContents::eInline);
			//TODO Bind pipeline
			SetViewport(cmd_buffer, vp_offset, vp_size);
			SetScissor(cmd_buffer, vp_offset, vp_size);
			//TODO DRAW

			VulkanPipeline* prev_pipeline = nullptr;
			vector<RscHandle<ShaderProgram>> shaders;
			for (auto& p_ro : task.DrawCalls())
			{
				//auto& obj = p_ro.Object();
				if (!p_ro.config)
					continue;
				

				auto& pipeline = (p_ro.vertex_shader==mesh_vtx)?_pimpl->mesh_pipeline.pipeline:_pimpl->skinned_mesh_pipeline.pipeline;
				
				if (prev_pipeline != &pipeline)
				{
					pipeline.Bind(cmd_buffer, View());
					prev_pipeline = &pipeline;
				}
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

			auto htex = fb.GetAttachment(0).buffer;
			auto& tex = htex.as<VknTexture>(); 
			auto layers = vk::ImageSubresourceLayers
			{
				vk::ImageAspectFlagBits::eColor,
				0,0,1
			};
			ivec2 point = ivec2{ data.point * vec2{ vp_size } };
			vk::BufferImageCopy copy
			{
				offset,0,0,layers,vk::Offset3D{point.x,(vp_size.y-1)-point.y,0},vk::Extent3D{1,1,1}
			};
			cmd_buffer.endRenderPass();
			hlp::TransitionImageLayout(cmd_buffer, {}, tex.Image(), tex.format, vk::ImageLayout::eGeneral, vk::ImageLayout::eTransferSrcOptimal);
			cmd_buffer.copyImageToBuffer(tex.Image(), vk::ImageLayout::eTransferSrcOptimal, result_buffer, copy);
			offset += picking_pixel_size;
		}
		cmd_buffer.end();
		auto fence = *fd.fence;

		vk::SubmitInfo su
		{
			0,nullptr,
			nullptr,
			1,&cmd_buffer,
			0,nullptr
		};
		View().GraphicsQueue().submit(su, fence);
		RdocEndFrameCapture();
	}

}