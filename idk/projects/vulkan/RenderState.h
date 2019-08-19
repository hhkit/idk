#pragma once
#include <idk.h>
#include <VectorBuffer.h>
#include <VulkanPipeline.h>
namespace idk::vgfx
{
	struct DrawCall
	{
		using first_binding_t = uint32_t;
		using offset_t        = uint32_t;
		idk::vector<std::pair<first_binding_t, offset_t>> vtx_binding{};
		//probably something for index too
		idk::vector<std::pair<first_binding_t, offset_t>> idx_binding{};
		//Probably something for UBO too
		uint32_t instance_count{}, vertex_count{};
		idk::VulkGfxPipeline* pipeline;
		idk::uniform_info     uniform_info;
	};
	class VulkanDetail;
	struct RenderState
	{
		struct MasterBuffer
		{
			idk::string             buffer{};
			//vk::UniqueBuffer        device_buffer;
			VectorBuffer            host_buffer{};

			uint32_t Add(const void* data, size_t len);
			void Reset();
			void UpdateBuffer(VulkanDetail& detail, RenderState& state);
		};
		MasterBuffer            master_buffer{};
		vk::RenderPass          render_pass{};
		vk::UniqueCommandBuffer	command_buffer{};
		vk::UniqueCommandBuffer	transfer_buffer{};
		idk::vector<DrawCall>   draw_calls{};

		void UpdateMasterBuffer(VulkanDetail& detail) { MasterBuffer().UpdateBuffer(detail, *this); }
		void AddDrawCall(DrawCall call);
		MasterBuffer& MasterBuffer() { return master_buffer; }
		vk::Buffer& Buffer() { return master_buffer.host_buffer.Buffer(); }
		vk::RenderPass& RenderPass() { return render_pass; }
		vk::CommandBuffer& TransferBuffer() { return *command_buffer; }// transfer_buffer; }
		vk::CommandBuffer& CommandBuffer() { return *command_buffer; }

	};

}