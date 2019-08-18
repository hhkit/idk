#include "pch.h"
#include "RenderState.h"
#include <VulkanDetail.h>
#include <VulkanHelpers.h>
namespace vgfx
{
	uint32_t RenderState::MasterBuffer::Add(const void* data, size_t len)
	{
		uint32_t start_index = idk::s_cast<uint32_t>(buffer.size());
		const char* ptr = idk::s_cast<const char*>(data);
		buffer.append(ptr, len);
		return start_index;
	}
	void RenderState::MasterBuffer::Reset()
	{
		buffer.resize(0);
	}
	void RenderState::MasterBuffer::UpdateBuffer(VulkanDetail& detail, RenderState& state)
	{
		host_buffer.Resize(detail.PDevice(), *detail.Device(), buffer.size());
		auto sz = std::size(buffer);
		auto dat = std::data(buffer);
		host_buffer.Update(0, vhlp::make_array_proxy(idk::s_cast<uint32_t>(sz), dat), state.TransferBuffer());
	}

	void RenderState::AddDrawCall(DrawCall call)
	{
	}

}