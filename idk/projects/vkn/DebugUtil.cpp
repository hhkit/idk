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
	namespace hlp
	{
		string DumpAllocators();
	}
}
namespace idk::vkn::dbg
{
	static bool enable_named_object = false;;
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

	void BeginLabel(vk::Queue queue, const char* label, const color& col)
	{
		auto dbg_label = CreateLabel(label, col);
		if (View().DynDispatcher().vkQueueBeginDebugUtilsLabelEXT)
			queue.beginDebugUtilsLabelEXT(dbg_label, View().DynDispatcher());
	}

	void EndLabel(vk::Queue queue)
	{
		if (View().DynDispatcher().vkQueueEndDebugUtilsLabelEXT)
			queue.endDebugUtilsLabelEXT(View().DynDispatcher());
	}




	void NameObject(vk::Image img, string_view name)
	{
		//return; //There's a bug in vulkan sdk up to version 1.1.121 or something where they don't copy the string.
		if (enable_named_object&&View().DynDispatcher().vkSetDebugUtilsObjectNameEXT)
		{
			vk::DebugUtilsObjectNameInfoEXT tmp
			{
				vk::ObjectType::eImage,reinterpret_cast<uint64_t>(img.operator VkImage()),name.data()
			};
			auto tmp_ = tmp.operator VkDebugUtilsObjectNameInfoEXT & ();

			View().DynDispatcher().vkSetDebugUtilsObjectNameEXT(*View().Device(), &tmp_);
		}
	}
	void NameObject(vk::Buffer img, string_view name)
	{
		//return; //There's a bug in vulkan sdk up to version 1.1.121 or something where they don't copy the string.
		if (enable_named_object && View().DynDispatcher().vkSetDebugUtilsObjectNameEXT)
		{
			vk::DebugUtilsObjectNameInfoEXT tmp
			{
				vk::ObjectType::eBuffer,reinterpret_cast<uint64_t>(img.operator VkBuffer()),name.data()
			};
			auto tmp_ = tmp.operator VkDebugUtilsObjectNameInfoEXT & ();

			View().DynDispatcher().vkSetDebugUtilsObjectNameEXT(*View().Device(), &tmp_);
		}
	}
	void NameObject(vk::RenderPass rp, string_view name)
	{
		//return; //There's a bug in vulkan sdk up to version 1.1.121 or something where they don't copy the string.
		if (enable_named_object && View().DynDispatcher().vkSetDebugUtilsObjectNameEXT)
		{
			vk::DebugUtilsObjectNameInfoEXT tmp
			{
				vk::ObjectType::eRenderPass,reinterpret_cast<uint64_t>(rp.operator VkRenderPass()),name.data()
			};
			auto tmp_ = tmp.operator VkDebugUtilsObjectNameInfoEXT & ();

			View().DynDispatcher().vkSetDebugUtilsObjectNameEXT(*View().Device(), &tmp_);
		}
	}
	void NameObject(vk::Framebuffer fb, string_view name)
	{
		//return; //There's a bug in vulkan sdk up to version 1.1.121 or something where they don't copy the string.
		if (enable_named_object && View().DynDispatcher().vkSetDebugUtilsObjectNameEXT)
		{
			vk::DebugUtilsObjectNameInfoEXT tmp
			{
				vk::ObjectType::eFramebuffer,reinterpret_cast<uint64_t>(fb.operator VkFramebuffer()),name.data()
			};
			auto tmp_ = tmp.operator VkDebugUtilsObjectNameInfoEXT & ();

			View().DynDispatcher().vkSetDebugUtilsObjectNameEXT(*View().Device(), &tmp_);
		}
	}
	void NameObject(uint64_t unk, string_view name)
	{
		//return; //There's a bug in vulkan sdk up to version 1.1.121 or something where they don't copy the string.
		if (enable_named_object && View().DynDispatcher().vkSetDebugUtilsObjectNameEXT && unk)//id must be valid if type is unknown
		{
			vk::DebugUtilsObjectNameInfoEXT tmp
			{
				vk::ObjectType::eUnknown,unk,name.data()
			};
			auto tmp_ = tmp.operator VkDebugUtilsObjectNameInfoEXT & ();

			View().DynDispatcher().vkSetDebugUtilsObjectNameEXT(*View().Device(), &tmp_);
		}
	}
	string num_bytes_to_str(size_t num_bytes)
	{
		const std::pair<float, string> post_fix[] =
		{
			{1000000000.0f," GB"},
			{1000000.0f," MB"},
			{1000.0f," KB"},
			//{1,"B"},
		};

		string result = std::to_string(num_bytes);
		string runit = " B";
		for (auto& [denominator, unit] : post_fix)
		{
			if (denominator < num_bytes)
			{
				result = std::to_string(num_bytes / denominator);
				runit = unit;
				break;
			}
		}
		return result + runit;
	}
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
				out << "\t\t Attachment[" << i << "] size: " << num_bytes_to_str(sz);
				if (!tracked_texture.emplace(tex).second)
					out << "[Duplicate]";
				else
				{
					fb_size += sz;
				}
				out << "\n";
			}
			out << "\t Framebuffer <" << fb.Name() << "> size: " << num_bytes_to_str(fb_size) << "\n";
			total_size += fb_size;
		}
		out << "All size: " << num_bytes_to_str(total_size) << "\n";
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
				out << "\t\t Color Attachment size: " << num_bytes_to_str(sz);
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
				out << "\t\t Depth Attachment size: " << num_bytes_to_str(sz);
				if (!tracked_texture.emplace(tex).second)
					out << "[Duplicate]";
				else
				{
					fb_size += sz;
				}
				out << "\n";
			}
			out << "\t RenderTarget <" << fb.Name() << "> size: " << num_bytes_to_str(fb_size) << "\n";
			total_size += fb_size;
		}
		out << "All size: " << num_bytes_to_str(total_size) << "\n";
		string derp = out.str();
		return derp;
	}

	string DumpMemoryAllocs()
	{
		return hlp::DumpAllocators();
	}
}