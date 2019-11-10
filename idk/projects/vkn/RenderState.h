#pragma once
#include <idk.h>
#include <vkn/vector_buffer.h>
#include <vkn/draw_call.h>
namespace idk::vkn
{
	class VulkanView;

	struct RenderState
	{
	public:
		struct master_buffer
		{
			string              buffer{};
			hlp::vector_buffer  host_buffer{};

			uint32_t add(const void* data, size_t len);
			void     reset();
			void     update_buffer(VulkanView& detail, RenderState& state);
		};

		// getters
		master_buffer&     MasterBuffer  () { return _master_buffer; }
		vk::Buffer         Buffer        () { return _master_buffer.host_buffer.buffer(); }
		vk::RenderPass&    RenderPass    () { return _render_pass; }
		vk::CommandBuffer& TransferBuffer() { return *_transfer_buffer; }
		vk::CommandBuffer& CommandBuffer () { return *_command_buffer; }
		vector<draw_call>& DrawCalls     () { return _draw_calls; };

		// setters
		void TransferBuffer(vk::UniqueCommandBuffer&& val) { _transfer_buffer = std::move(val); }
		void CommandBuffer(vk::UniqueCommandBuffer&& val) { _command_buffer = std::move(val); }

		// modifiers
		void AddDrawCall(draw_call call);
		void UpdateMasterBuffer(VulkanView& detail) { MasterBuffer().update_buffer(detail, *this); }

	private:
		master_buffer            _master_buffer{};
		vk::RenderPass           _render_pass{};
		vk::UniqueCommandBuffer  _command_buffer{};
		vk::UniqueCommandBuffer  _transfer_buffer{};
		vector<draw_call>        _draw_calls{};
	};

}