#include "pch.h"
#include "RenderState.h"
#include <vkn/VulkanView.h>
#include <vkn/BufferHelpers.h>
namespace idk::vkn
{
	uint32_t RenderState::master_buffer::add(const void* data, size_t len)
	{
		uint32_t start_index = idk::s_cast<uint32_t>(buffer.size());
		const char* ptr = idk::s_cast<const char*>(data);
		buffer.append(ptr, len);
		return start_index;
	}
	void RenderState::master_buffer::reset()
	{
		buffer.resize(0);
	}
	void RenderState::master_buffer::update_buffer(VulkanView& detail, RenderState& state)
	{
		host_buffer.resize(detail.PDevice(), *detail.Device(), buffer.size());
		auto sz = std::size(buffer);
		auto dat = std::data(buffer);
		host_buffer.update(0, hlp::make_array_proxy(idk::s_cast<uint32_t>(sz), dat), state.TransferBuffer());
	}

	void RenderState::AddDrawCall(draw_call call)
	{
		_draw_calls.emplace_back(std::move(call));
	}

}