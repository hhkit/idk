#include "pch.h"
#include "VknRenderTarget.h"
#include <core/Core.h>
#include <vkn/VknTexture.h>
#include <vkn/VulkanWin32GraphicsSystem.h>

#include <vkn/VknTextureLoader.h>

#include <vkn/VknRenderTargetFactory.h>
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>

#include <vkn/DebugUtil.h>

#include <gfx/FramebufferFactory.h>

namespace idk::vkn
{

	size_t permutation_index(bool clear_col, bool clear_depth)
	{
		return ((size_t)clear_col) | (((size_t)clear_depth) << 1);
	}
	std::pair<bool,bool> permutations_from_index(size_t index)
	{
		return { (bool)(index&1),(bool)(index>>1)};
	}

	void VknRenderTarget::OnFinalize()
	{
		for (auto& elem : Textures())
		{
			if (elem != RscHandle<Texture>{})
			{
				elem = RscHandle<Texture>{Core::GetResourceManager().LoaderEmplaceResource<VknTexture>(elem.guid)};
				Core::GetResourceManager().Release(elem);
			}
		}
		if (_frame_buffer)
		{
			Core::GetResourceManager().Release(_frame_buffer);
		}
		FrameBufferBuilder fbf{};
		fbf.Begin(Name(), Size(), 1);
		auto col_info =
			AttachmentInfo
		{
			LoadOp::eClear,
			StoreOp::eStore,
			(Srgb()) ? TextureInternalFormat::SBGRA_8 : TextureInternalFormat::BGRA_8,
			FilterMode::Nearest,
			false,
			(color_tex.guid != Guid{}) ? AttachmentInfo::buffer_opt_t{color_tex.guid} : std::nullopt
		};
		auto dep_info = AttachmentInfo
		{
			LoadOp::eClear,
			StoreOp::eStore,
			TextureInternalFormat::DEPTH_32,
			FilterMode::Nearest,
			false,
			(depth_tex.guid != Guid{}) ? AttachmentInfo::buffer_opt_t{depth_tex.guid} : std::nullopt
		};
		fbf.AddAttachment(col_info);
		fbf.SetDepthAttachment(dep_info);
		{
			auto& factory = Core::GetResourceManager().GetFactory<FrameBufferFactory>();
			auto info = fbf.End();
			_frame_buffer = factory.Create(info);

			col_info.buffer = color_tex = _frame_buffer.as<VknFrameBuffer>().GetAttachment  (0).buffer;
			dep_info.buffer = depth_tex = _frame_buffer.as<VknFrameBuffer>().DepthAttachment( ).buffer;

			auto& col = color_tex.as<VknTexture>();
			auto& dep = depth_tex.as<VknTexture>();
			dbg::NameObject(col.Image(), col.Name());
			dbg::NameObject(dep.Image(), dep.Name());

			auto max = permutation_index(true, true) + 1;
			_permutations.resize(max);
			for (auto i = max; i-- > 0;)
			{
				fbf.Begin(Name(), Size(), 1);
				auto [clear_col, clear_dep] = permutations_from_index(i);
				col_info.load_op = (clear_col) ? LoadOp::eClear : LoadOp::eLoad;
				fbf.AddAttachment(col_info);
				dep_info.load_op = (clear_dep) ? LoadOp::eClear : LoadOp::eLoad;
				fbf.SetDepthAttachment(dep_info);
				info = fbf.End();
				auto& perm = _permutations[i];
				if (perm)
					Core::GetResourceManager().Release(perm);
				perm = (factory.Create(info));
				
			}

		}
		Saveable_t::Dirty();
		////TODO get these from somewhere persistent.
		//auto& factory = Core::GetResourceManager().GetFactory<VknRenderTargetFactory>();
		//hlp::MemoryAllocator& alloc = factory.GetAllocator();
		//vk::Fence fence = factory.GetFence();

		////Reload the textures with the new configuration.
		//TextureLoader loader;


		////TODO store a framebuffer instead.
		//auto color_texture = Core::GetResourceManager().LoaderEmplaceResource<VknTexture>(GetColorBuffer().guid);
		//auto ctci = ColorBufferTexInfo(s_cast<uint32_t>(size.x), s_cast<uint32_t>(size.y));
		//ctci.internal_format = (Srgb()) ? vk::Format::eB8G8R8A8Srgb : vk::Format::eB8G8R8A8Unorm;
		//ctci.image_usage |= vk::ImageUsageFlagBits::eInputAttachment;
		//loader.LoadTexture(*color_texture, alloc, fence, {}, ctci, {});
		//auto depth_texture = Core::GetResourceManager().LoaderEmplaceResource<VknTexture>(GetDepthBuffer().guid);
		//TexCreateInfo dtci = DepthBufferTexInfo(s_cast<uint32_t>(size.x), s_cast<uint32_t>(size.y));
		//dtci.image_usage |= vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eInputAttachment | vk::ImageUsageFlagBits::eDepthStencilAttachment;
		//loader.LoadTexture(*depth_texture, alloc, fence, {}, dtci, {});

		//auto& col = *color_texture;
		//auto& dep = *depth_texture;
		//{ 
		//	VulkanView& vknView = View();
		//	const vk::ImageView image_views[] = {color_texture->ImageView(),depth_texture->ImageView()};

		//	vk::FramebufferCreateInfo framebufferInfo = {};
		//	framebufferInfo.renderPass = *vknView.BasicRenderPass(rp_type);
		//	framebufferInfo.attachmentCount = hlp::arr_count(image_views);
		//	framebufferInfo.pAttachments = std::data(image_views);
		//	framebufferInfo.width  = s_cast<uint32_t>(size.x);
		//	framebufferInfo.height = s_cast<uint32_t>(size.y);
		//	framebufferInfo.layers = 1;

		//	buffer = vknView.Device()->createFramebufferUnique(framebufferInfo, nullptr, vknView.Dispatcher());

		//}
	}
	
	void TransitionTexture(vk::CommandBuffer cmd_buffer, vk::ImageLayout type, VknTexture& tex)
	{
		hlp::TransitionImageLayout(cmd_buffer, View().GraphicsQueue(), tex.Image(), tex.format, vk::ImageLayout::eUndefined, type);
	}

	void VknRenderTarget::PrepareDraw(vk::CommandBuffer& )
	{
		auto& col = GetColorBuffer().as<VknTexture>();
		auto& dep = GetDepthBuffer().as<VknTexture>();
		dbg::NameObject(col.Image(), col.Name());
		dbg::NameObject(dep.Image(), dep.Name());
		//TransitionTexture(cmd_buffer, vk::ImageLayout::eColorAttachmentOptimal       , col);
		//TransitionTexture(cmd_buffer, vk::ImageLayout::eDepthStencilAttachmentOptimal, dep);
	}

	RenderPassObj VknRenderTarget::GetRenderPass(bool clear_col , bool clear_depth ) const
	{
		return _permutations[permutation_index(clear_col,clear_depth)].as<VknFrameBuffer>().GetRenderPass();
	}

	vk::Framebuffer VknRenderTarget::Buffer()
	{
		return _frame_buffer.as<VknFrameBuffer>().GetFramebuffer();
	}

	vk::Semaphore VknRenderTarget::ReadySignal()
	{
		return *ready_semaphore;
	}


	/*GLuint VknFrameBuffer::DepthBuffer() const
	{
		return depthbuffer;
	}*/

}