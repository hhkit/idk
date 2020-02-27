#pragma once
#include <vulkan/vulkan.hpp>
#include <idk.h>
namespace idk::vkn::dbg
{
	//Don't use string_view, vulkan takes in a c_string so we can't tell it how long the string really is.
	void AddLabel(vk::CommandBuffer cmd_buffer, const char* label, const color& col = idk::color{ 0,0,1,1 });
	void BeginLabel(vk::CommandBuffer cmd_buffer, const char*, const color& col = idk::color{ 0,0,1,1 });
	void EndLabel(vk::CommandBuffer cmd_buffer);
	void BeginLabel(vk::Queue queue, const char*, const color& col = idk::color{ 0,0,1,1 });
	void EndLabel(vk::Queue queue);

	void NameObject(vk::Image img, string_view name);
	void NameObject(vk::Buffer buf, string_view name);
	void NameObject(vk::RenderPass rp, string_view name);
	void NameObject(vk::Framebuffer rp, string_view name);
	void NameObject(uint64_t unk, string_view name);

	string DumpFrameBufferAllocs();
	string DumpRenderTargetAllocs();
}
