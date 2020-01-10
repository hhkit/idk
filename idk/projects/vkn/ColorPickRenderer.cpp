#include "pch.h"
#include "ColorPickRenderer.h"
#include <vkn/PipelineThingy.h>

#include <vulkan/vulkan.hpp>
#include <vkn/VulkanView.h>
#include <vkn/VknRenderTarget.h>
#include <Core/Core.inl>
#include <res/ResourceManager.inl>	
#include <vkn/MemoryAllocator.h>

#include <vkn/utils/VknUtil.h>
namespace idk::vkn
{
	constexpr size_t picking_pixel_size = sizeof(uint32_t);
	VulkanView& View();
	struct RequestInfo
	{
		ColorPickRequest request;
	};

	template<size_t src_size,typename dest_t,size_t dst_size = sizeof(dest_t)>
	dest_t adjust(const byte* start)
	{
		byte ro_index_buffer[std::max(picking_pixel_size, sizeof(uint32_t))] = {};
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
		void Fulfill()
		{
			vk::Device d = *View().Device();
			const byte* memory_range = static_cast<byte*>(d.mapMemory(buffer, offset, picking_pixel_size*requests.size(), vk::MemoryMapFlags{}));
			size_t i = 0;
			for (auto& req : requests)
			{
				auto ro_index = adjust<picking_pixel_size,uint32_t>(memory_range + picking_pixel_size * i);
				req.request.select(ro_index); 
				++i;
			}
		}

		vk::UniqueFence fence;
		vk::DeviceMemory buffer;
		size_t offset;
		vector<RequestInfo> requests;
	};
	struct FrameDataHandler
	{
		std::forward_list<FrameData> data;
		vk::UniqueFence MakeFence();
		FrameData& Add(vector<ColorPickRequest> && requests)
		{
			auto& fd = data.emplace_front(FrameData{ MakeFence() });
			fd.requests.reserve(requests.size());
			std::move(requests.begin(), requests.end(), std::back_inserter(fd.requests));
			requests.clear();
			return fd;
		}
		void FreeFrameBuffers(size_t pool_start, size_t pool_end);
		std::pair<size_t, size_t> AcquireFrameBuffers(size_t num_buffers);
		void Check()
		{
			while (!data.empty() && data.front().Done())
			{
				auto& fd = data.front();
				fd.Fulfill();
				FreeFrameBuffers(fd.frame_buffer_start, fd.frame_buffer_end);
				data.pop_front();
			}
		}
	};

	struct ColorPickRenderer::PImpl
	{
		FrameDataHandler handler;
		hlp::vector_buffer id_buffer;
		RscHandle<VknFrameBuffer> frame_buffer;
		hlp::MemoryAllocator allocator;
	};
	ColorPickRenderer::ColorPickRenderer() :_pimpl{ std::make_unique<PImpl>() }
	{

	}
	ColorPickRenderer::~ColorPickRenderer() = default;

	//total_num_insts = total number of instances stored in inst_mesh_render_buffer
	void ColorPickRenderer::PreRender(vector<ColorPickRequest>& requests, const  SharedGraphicsState& shared_gs, size_t total_num_insts, vk::CommandBuffer cmd_buffer)
	{
		vector<uint32_t> id_buffer(total_num_insts);
		ivec2 max_size{};
		for (auto& request : requests)
		{
			auto& render_data = request.data;
			uint32_t id = 0;
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
			//TODO skinned_mesh_renderers, continue using id
		}
		_pimpl->id_buffer.resize(hlp::buffer_size(id_buffer));
		_pimpl->id_buffer.update<uint32_t>(0, id_buffer, cmd_buffer);

		auto curr_fb_size = _pimpl->frame_buffer->Size();
		if (curr_fb_size.x < max_size.x || curr_fb_size.y < max_size.y)
		{
			FrameBufferBuilder fbf;
			fbf.Begin(max_size, 1);
			fbf.AddAttachment(
				AttachmentInfo
				{
					LoadOp::eClear,StoreOp::eStore,ColorFormat::RUI_32,FilterMode::_enum::Nearest
				}
			);
			fbf.SetDepthAttachment(
				AttachmentInfo
				{
					LoadOp::eClear,StoreOp::eStore,ColorFormat::DEPTH_COMPONENT,FilterMode::_enum::Nearest
				}
			);
			auto& rm = Core::GetResourceManager();
			if (_pimpl->frame_buffer)
				rm.Release(_pimpl->frame_buffer);
			auto handle = rm.GetFactory<FrameBufferFactory>().Create(fbf.End());
			_pimpl->frame_buffer = RscHandle<VknFrameBuffer>{ handle };
		}
		
	}

	//Main Tasks left:
	//1)Change the render_data stuff to be non-instanced
	//2)Set up the appropriate render_pass
	//3)Set up the appropriate transfer buffer
	void ColorPickRenderer::Render(vector<ColorPickRequest>& requests, const  SharedGraphicsState& shared_gs, RenderStateV2& rs)
	{
		auto& color_pick_frag = shared_gs.renderer_vertex_shaders[FPicking];
		auto& mesh_vtx = shared_gs.renderer_vertex_shaders[VNormalMeshPicker];
		auto& skinned_mesh_vtx = shared_gs.renderer_vertex_shaders[VSkinnedMeshPicker];
		vector<PipelineThingy> render_tasks;
		for (auto& request : requests)
		{
			
			PipelineThingy&  the_interface = render_tasks.emplace_back();
			the_interface.SetRef(rs.ubo_manager);
			auto& render_data = request.data;
			the_interface.BindShader(ShaderStage::Vertex, mesh_vtx);
			the_interface.BindShader(ShaderStage::Fragment, color_pick_frag);
			uint32_t i = 0;
			the_interface.BindUniformBuffer("PerCamera", 0, render_data.camera.projection_matrix);
			for (auto itr = (*shared_gs.instanced_ros).data()+ render_data.inst_mesh_render_begin, end = (*shared_gs.instanced_ros).data() + render_data.inst_mesh_render_end;itr<end;++itr)
			{
				auto& ro = *itr;
				the_interface.BindUniformBuffer("id", 0, i);
				//the_interface.BindUniformBuffer("PerObject", 0, render_data.camera.view_matrix*ro.transform);
				the_interface.BindAttrib(4, shared_gs.inst_mesh_render_buffer.buffer(), 0);
				the_interface.BindAttrib(1, _pimpl->id_buffer.buffer(), 0);
				the_interface.BindMeshBuffers(ro);
				the_interface.FinalizeDrawCall(ro,ro.num_instances,ro.instanced_index);
			}
			//TODO skinned stuff
		}
		size_t i = 0;
		auto req_itr = requests.begin();
		//TODO Acquire framebuffers
		vk::CommandBuffer cmd_buffer = *rs.cmd_buffer;
		cmd_buffer.begin(vk::CommandBufferBeginInfo{vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
		//TODO actually configure a proper result buffer
		vk::Buffer result_buffer;
		size_t offset = 0;//TODO Use a proper offset
		auto& fb = _pimpl->frame_buffer.as<VknRenderTarget>();
		auto&& rp = fb.GetRenderPass(true, true);
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
		for (auto& task : render_tasks)
		{
			auto& request = *req_itr;
			auto& data = request.data;
			task.GenerateDS(rs.dpools, (++i)==render_tasks.size());
			//TODO Bind FrameBuffers
			//TODO beginRenderPass
			vk::RenderPassBeginInfo rpbi
			{
				*rp,fb.Buffer(),
				hlp::ToRect2D({},data.camera.render_target->Size()),
				static_cast<uint32_t>(clear_val.size()),clear_val.data()
			};
			cmd_buffer.beginRenderPass(rpbi,vk::SubpassContents::eInline);
			//TODO Draw using PipelineThingy's info
			
			RscHandle<VknTexture> htex;//TODO grab tex from render buffer
			auto& tex = htex.as<VknTexture>(); 
			auto layers = vk::ImageSubresourceLayers
			{};
			vk::BufferImageCopy copy
			{
				offset,0,0,layers,vk::Offset3D{data.point.x,data.point.y,0},vk::Extent3D{1,1,1}
			};
			cmd_buffer.copyImageToBuffer(tex.Image(), vk::ImageLayout::eTransferSrcOptimal, result_buffer, copy);
			cmd_buffer.endRenderPass();
			offset += picking_pixel_size;
		}
		cmd_buffer.end();
		auto& fd = _pimpl->handler.Add(std::move(requests));
		auto fence = *fd.fence;
		//TODO submit buffer and set it up to signal the fence
		//TODO update framebuffer range
	}

}