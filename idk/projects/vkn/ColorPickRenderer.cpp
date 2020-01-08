#include "pch.h"
#include "ColorPickRenderer.h"
#include <vkn/PipelineThingy.h>

#include <vulkan/vulkan.hpp>
#include <vkn/VulkanView.h>

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
		size_t frame_buffer_start, frame_buffer_end;
	};

	struct ColorPickRenderer::PImpl
	{
		std::forward_list<FrameData> data;
		vk::UniqueFence MakeFence();
		FrameData& Add(vector<ColorPickRequest>&& requests)
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
			while (!data.empty()&&data.front().Done())
			{
				auto& fd = data.front();
				fd.Fulfill();
				FreeFrameBuffers(fd.frame_buffer_start,fd.frame_buffer_end);
				data.pop_front();
			}
		}
	};
	ColorPickRenderer::ColorPickRenderer() :_pimpl{ std::make_unique<PImpl>() }
	{

	}
	ColorPickRenderer::~ColorPickRenderer() = default;
	void ColorPickRenderer::Render(vector<ColorPickRequest>& requests, SharedGraphicsState& shared_gs, RenderStateV2& rs)
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
			size_t i = 0;
			for (auto itr = (*shared_gs.instanced_ros).data()+ render_data.inst_mesh_render_begin, end = (*shared_gs.instanced_ros).data() + render_data.inst_mesh_render_end;itr<end;++itr)
			{
				auto& inst_ro = *itr;
				the_interface.BindUniformBuffer("id", 0, i);
				the_interface.BindMeshBuffers(inst_ro);
				the_interface.BindAttrib(4, shared_gs.inst_mesh_render_buffer.buffer(), 0);
				the_interface.FinalizeDrawCall(inst_ro, inst_ro.num_instances, inst_ro.instanced_index);
			}
			//TODO skinned stuff
		}
		size_t i = 0;
		auto req_itr = requests.begin();
		//TODO Acquire framebuffers
		auto [fb_start,fb_end]=_pimpl->AcquireFrameBuffers(requests.size());
		vk::CommandBuffer cmd_buffer = *rs.cmd_buffer;
		cmd_buffer.begin(vk::CommandBufferBeginInfo{vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
		//TODO actually configure a proper result buffer
		vk::Buffer result_buffer;
		size_t offset = 0;//TODO Use a proper offset
		for (auto& task : render_tasks)
		{
			auto& request = *req_itr;
			task.GenerateDS(rs.dpools, (++i)==render_tasks.size());
			//TODO Bind FrameBuffers
			//TODO beginRenderPass
			cmd_buffer.beginRenderPass();
			//TODO Draw using PipelineThingy's info
			
			auto& data = request.data;
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
		auto& fd = _pimpl->Add(std::move(requests));
		auto fence = *fd.fence;
		//TODO submit buffer and set it up to signal the fence
		//TODO update framebuffer range
		fd.frame_buffer_start = fb_start;
		fd.frame_buffer_end = fb_end;
	}

}