#include "pch.h"
#include "DebugUtil.h"
#include <vkn/VulkanView.h>

#include <sstream>
#include <vkn/VknFrameBuffer.h>
#include <vkn/VknRenderTarget.h>
#include <vkn/VknTexture.h>
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>
namespace idk::vkn
{
	VulkanView& View();
}
namespace idk::vkn::dbg
{

	auto CreateLabel(const char* label, const color& col)
	{
		return vk::DebugUtilsLabelEXT
		{
			label,*r_cast<const std::array<float,4>*>(col.data())
		};
	}

	void AddLabel(vk::CommandBuffer cmd_buffer, const char* label, const color& color)
	{
		auto dbg_label = CreateLabel(label, color);
		if (View().DynDispatcher().vkCmdInsertDebugUtilsLabelEXT)
			cmd_buffer.insertDebugUtilsLabelEXT(dbg_label, View().DynDispatcher());
	}

	void BeginLabel(vk::CommandBuffer cmd_buffer, const char* label, const color& color)
	{
		auto dbg_label = CreateLabel(label, color);
		if (View().DynDispatcher().vkCmdBeginDebugUtilsLabelEXT)
			cmd_buffer.beginDebugUtilsLabelEXT(dbg_label, View().DynDispatcher());
	}

	void EndLabel(vk::CommandBuffer cmd_buffer)
	{
		if (View().DynDispatcher().vkCmdEndDebugUtilsLabelEXT)
			cmd_buffer.endDebugUtilsLabelEXT(View().DynDispatcher());
	}
// #pragma optimize("",off)
	string DumpFrameBufferAllocs()
	{
		std::stringstream out;
		auto fbs = Core::GetResourceManager().GetAll<VknFrameBuffer>();
		hash_set<RscHandle<Texture>> tracked_texture;
		size_t total_size = 0;
		for (auto& hfb : fbs)
		{
			auto& fb = *hfb;
			size_t fb_size = 0;
			for (size_t i = 0, end = fb.NumColorAttachments(); i < end; ++i)
			{
				auto tex = fb.GetAttachment(i).buffer;

				auto sz = tex.as<VknTexture>().sizeOnDevice;
				out << "\t\t Attachment[" << i << "] size: " << sz;
				if (!tracked_texture.emplace(tex).second)
					out << "[Duplicate]";
				else
				{
					fb_size += sz;
				}
				out << "\n";
			}
			out << "\t Framebuffer <" << fb.Name() << "> size: " << fb_size << "\n";
			total_size += fb_size;
		}
		out << "All size: " << total_size << "\n";
		string derp = out.str();
		return derp;
	}
	string DumpRenderTargetAllocs()
	{
		std::stringstream out;
		auto fbs = Core::GetResourceManager().GetAll<VknRenderTarget>();
		hash_set<RscHandle<Texture>> tracked_texture;
		size_t total_size = 0;
		for (auto& hfb : fbs)
		{
			auto& fb = *hfb;
			size_t fb_size = 0;
			//for (size_t i = 0, end = fb.NumColorAttachments(); i < end; ++i)
			{
				auto tex = fb.GetColorBuffer();

				auto sz = tex.as<VknTexture>().sizeOnDevice;
				out << "\t\t Color Attachment size: " << sz;
				if (!tracked_texture.emplace(tex).second)
					out << "[Duplicate]";
				else
				{
					fb_size += sz;
				}
				out << "\n";
			}
			{
				auto tex = fb.GetDepthBuffer();

				auto sz = tex.as<VknTexture>().sizeOnDevice;
				out << "\t\t Depth Attachment size: " << sz;
				if (!tracked_texture.emplace(tex).second)
					out << "[Duplicate]";
				else
				{
					fb_size += sz;
				}
				out << "\n";
			}
			out << "\t RenderTarget <" << fb.Name() << "> size: " << fb_size << "\n";
			total_size += fb_size;
		}
		out << "All size: " << total_size << "\n";
		string derp = out.str();
		return derp;
	}
}