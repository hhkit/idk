#pragma once
#include <idk.h>
#include <vkn/VectorBuffer.h>
#include <vkn/VulkanPipeline.h>
namespace idk::vkn
{
	class VulkanView;
	struct DrawCall
	{
		using first_binding_t = uint32_t;
		using offset_t        = uint32_t;
		vector<std::pair<first_binding_t, offset_t>> vtx_binding{};
		//probably something for index too
		vector<std::pair<first_binding_t, offset_t>> idx_binding{};
		//Probably something for UBO too
		uint32_t instance_count{}, vertex_count{};
		VulkGfxPipeline* pipeline;
		uniform_info     uniform_info;
	};
	struct RenderState
	{
		struct MasterBuffer
		{
			string             buffer{};
			//vk::UniqueBuffer        device_buffer;
			hlp::VectorBuffer  host_buffer{};

			uint32_t Add(const void* data, size_t len);
			void Reset();
			void UpdateBuffer(VulkanView& detail, RenderState& state);
		};
		MasterBuffer            master_buffer{};
		vk::RenderPass          render_pass{};
		vk::UniqueCommandBuffer	command_buffer{};
		vk::UniqueCommandBuffer	transfer_buffer{};
		idk::vector<DrawCall>   draw_calls{};

		void UpdateMasterBuffer(VulkanView& detail) { MasterBuffer().UpdateBuffer(detail, *this); }
		void AddDrawCall(DrawCall call);
		MasterBuffer& MasterBuffer() { return master_buffer; }
		vk::Buffer& Buffer() { return master_buffer.host_buffer.Buffer(); }
		vk::RenderPass& RenderPass() { return render_pass; }
		vk::CommandBuffer& TransferBuffer() { return *command_buffer; }// transfer_buffer; }
		vk::CommandBuffer& CommandBuffer() { return *command_buffer; }

	};

}