#include "pch.h"
#include "VknFrameBuffer.h"
#include <core/Core.h>
#include <vkn/VknTexture.h>
#include <vkn/VulkanWin32GraphicsSystem.h>

#include <vkn/VknTextureLoader.h>

namespace idk::vkn
{
	VulkanView& View()
	{
		return Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View();
	}
	VknRenderTarget::VknRenderTarget(VknTexture iv, VulkanView& vknView)
		:buffer{},
		image{std::move(iv.image)},
		//imageView{std::move(iv.imageView)},
		size{iv.size}
	{
		imageView.emplace_back(*iv.imageView);
		//Creates an empty framebuffer tht does nothing at all until attach image views is called

		AttachImageViews(std::move(iv.image), imageView, vknView,size);
	}

	VknRenderTarget::VknRenderTarget(unique_ptr<VknTexture> iv, VulkanView& vknView)
		:buffer{},
		image{ std::move(iv->image) },
		//imageView{std::move(iv.imageView)},
		size{ iv->size }
	{
		imageView.emplace_back(std::move(*iv->imageView));
		//Creates an empty framebuffer tht does nothing at all until attach image views is called

		AttachImageViews(imageView, vknView,size);
	}

	VknRenderTarget::VknRenderTarget(vk::UniqueImage img, vector<vk::ImageView> iv, VulkanView& vknView, vec2 fbsize)
		:buffer{},
		image{ std::move(img) },
		imageView{iv},
		size{ fbsize }
	{
		vk::FramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.renderPass = *vknView.Renderpass();
		framebufferInfo.attachmentCount = hlp::arr_count(iv);
		framebufferInfo.pAttachments = std::data(iv);
		framebufferInfo.width = s_cast<uint32_t>(size.x);
		framebufferInfo.height = s_cast<uint32_t>(size.y);
		framebufferInfo.layers = 1;

		buffer = vknView.Device()->createFramebufferUnique(framebufferInfo, nullptr, vknView.Dispatcher());

		uncreated = false;
	}

	VknRenderTarget::VknRenderTarget(VknRenderTarget&& rhs)
		:RenderTarget{ std::move(rhs) },
		buffer {std::move(rhs.buffer)},
		image{ std::move(rhs.image) },
		imageView{ std::move(rhs.imageView) },
		size{ std::move(size) },
		uncreated{std::move(rhs.uncreated)}
	{}

	VknRenderTarget& VknRenderTarget::operator=(VknRenderTarget&& rhs)
	{
		// TODO: insert return statement here
		std::swap(buffer,rhs.buffer);
		std::swap(image,rhs.image);
		std::swap(imageView,rhs.imageView);
		std::swap(size,rhs.size);
		std::swap(uncreated,rhs.uncreated);

		return *this;
	}

	VknRenderTarget::~VknRenderTarget()
	{
		//for (auto& elem : meta.textures)
		//	if(elem)
		//		Core::GetResourceManager().Free(elem);
		
		//glDeleteRenderbuffers(1, &depthbuffer);
		buffer.reset();
		imageView.clear();
		image.reset();
	}

	void VknRenderTarget::OnMetaUpdate(const Metadata& newmeta)
	{
		for (auto& elem : meta.textures)
			Core::GetResourceManager().Free(elem);
		//TODO get these from somewhere persistent.
		hlp::MemoryAllocator* alloc;
		vk::Fence fence;
		/*glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, newmeta.size.x, newmeta.size.y);*/
		TextureLoader loader;
		auto color_tex = Core::GetResourceManager().LoaderEmplaceResource<VknTexture>(newmeta.textures[kColorIndex].guid);
		loader.LoadTexture(*color_tex, TextureFormat::eRGBA32, {}, nullptr, 0, newmeta.size, *alloc, fence, true);
		auto depth_tex = Core::GetResourceManager().LoaderEmplaceResource<VknTexture>(newmeta.textures[kDepthIndex].guid);
		loader.LoadTexture(*depth_tex, TextureFormat::eD16Unorm, {}, nullptr, 0, newmeta.size, *alloc, fence, true);
	}

	void VknRenderTarget::ReattachImageViews(VulkanView& vknView)
	{
		buffer.reset();
		vector<vk::ImageView> vimage_views;
		auto& mmeta = GetMeta();
		//TODO assert that all textures have to be the same size
		for (auto& im : mmeta.textures)
		{
			vimage_views.emplace_back(*im.as<VknTexture>().imageView);
		}

		vk::FramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.renderPass = vknView.BasicRenderPass(rp_type);
		framebufferInfo.attachmentCount = (uint32_t)vimage_views.size();
		framebufferInfo.pAttachments = std::data(vimage_views);
		framebufferInfo.width  = s_cast<uint32_t>(mmeta.size.x);
		framebufferInfo.height = s_cast<uint32_t>(mmeta.size.y);
		framebufferInfo.layers = 1;
		;
		ready_semaphore = vknView.Device()->createSemaphoreUnique(vk::SemaphoreCreateInfo{});

		buffer = vknView.Device()->createFramebufferUnique(framebufferInfo, nullptr, vknView.Dispatcher());

		size = vec2(mmeta.size);

		uncreated = false;
	}

	void VknRenderTarget::AttachImageViews(VknImageData& iv, VulkanView& vknView)
	{
		//One framebuffer can reference multiple attachments (color, position, light etc.)

		//We are using 3 swapchain images to represent colour attachment

		//Need create other kinds of attachment too in the renderpasses

		//for now vector iv should only have one colour attachment image
		/*vector<vk::ImageView> ref;
		for (auto& elem : iv)
			ref.emplace_back(*elem);*/

		buffer.reset();

		vk::FramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.renderPass = *vknView.Renderpass();
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = &*iv.imageView;
		framebufferInfo.width = s_cast<uint32_t>(iv.size.x);
		framebufferInfo.height = s_cast<uint32_t>(iv.size.y);
		framebufferInfo.layers = 1;

		buffer = vknView.Device()->createFramebufferUnique(framebufferInfo,nullptr,vknView.Dispatcher());

		size = vec2(iv.size);

		uncreated = false;
	}

	void VknRenderTarget::AttachImageViews(vk::UniqueImage img, vector<vk::ImageView> iv, VulkanView& vknView, vec2 siz)
	{
		//One framebuffer can reference multiple attachments (color, position, light etc.)

		//We are using 3 swapchain images to represent colour attachment

		//Need create other kinds of attachment too in the renderpasses

		//for now vector iv should only have one colour attachment image
		/*vector<vk::ImageView> ref;
		for (auto& elem : iv)
			ref.emplace_back(*elem);*/

		buffer.reset();

		vk::FramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.renderPass = *vknView.Renderpass();
		framebufferInfo.attachmentCount = hlp::arr_count(iv);
		framebufferInfo.pAttachments = std::data(iv);
		framebufferInfo.width  = s_cast<uint32_t>(siz.x);
		framebufferInfo.height = s_cast<uint32_t>(siz.y);
		framebufferInfo.layers = 1;

		buffer = vknView.Device()->createFramebufferUnique(framebufferInfo, nullptr, vknView.Dispatcher());

		size = vec2(siz);

		uncreated = false;
	}

	void VknRenderTarget::AttachImageViews(vk::RenderPass rp, vector<vk::ImageView> iv, VulkanView& vknView, vec2 siz)
	{
		vk::FramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.renderPass = rp;
		framebufferInfo.attachmentCount = hlp::arr_count(iv);
		framebufferInfo.pAttachments = std::data(iv);
		framebufferInfo.width = s_cast<uint32_t>(siz.x);
		framebufferInfo.height = s_cast<uint32_t>(siz.y);
		framebufferInfo.layers = 1;

		buffer = vknView.Device()->createFramebufferUnique(framebufferInfo, nullptr, vknView.Dispatcher());

		size = vec2(siz);

		uncreated = false;
	}

	void VknRenderTarget::AttachImageViews(vector<vk::ImageView> iv, VulkanView& vknView, vec2 siz)
	{
		AttachImageViews(*vknView.Renderpass(),iv, vknView, siz);
	}

	void TransitionTexture(vk::CommandBuffer cmd_buffer, AttachmentType type, VknTexture& tex)
	{
		hlp::TransitionImageLayout(true, cmd_buffer, View().GraphicsQueue(), *tex.image, tex.format, vk::ImageLayout::eUndefined, (type == AttachmentType::eColor) ? vk::ImageLayout::eColorAttachmentOptimal : vk::ImageLayout::eDepthStencilAttachmentOptimal);
	}

	void VknRenderTarget::PrepareDraw(vk::CommandBuffer& cmd_buffer)
	{
		for (AttachmentType type{};type<AttachmentType::eSizeAT;++type)
		{
			for (auto& index : attachments[type])
			{
				TransitionTexture(cmd_buffer,type, GetMeta().textures[index].as<VknTexture>());

			}
		}
	}

	vk::RenderPass VknRenderTarget::GetRenderPass() const
	{
		return View().BasicRenderPass(rp_type);
	}

	vk::Framebuffer VknRenderTarget::Buffer()
	{
		return *buffer;
	}

	vk::Semaphore VknRenderTarget::ReadySignal()
	{
		return *ready_semaphore;
	}

	void VknRenderTarget::AddAttachmentImpl(AttachmentType type, RscHandle<Texture> texture)
	{
		TextureLoader loader;
		auto ptr = RscHandle<VknTexture>(texture);
		auto sz = texture->Size();
		switch (type)
		{ 
		case AttachmentType::eColor:
			loader.LoadTexture(*ptr, TextureFormat::eBGRA32, {}, nullptr, s_cast<size_t>(4 * sz.x * sz.y), ivec2{ sz }, *allocator, fence, true);
		break;
		case AttachmentType::eDepth:
			auto depth_ptr = ptr;
			loader.LoadTexture(*depth_ptr, TextureFormat::eD16Unorm, {}, nullptr, s_cast<size_t>(2 * sz.x * sz.y), ivec2{ sz }, *allocator, fence, true);
			break;
		}
		//auto& test = texture.as<VknTexture>();
	}

	void VknRenderTarget::Finalize()
	{
		vector<vk::ImageView> image_views_;
		vec2 sz;
		for (auto& tex : GetMeta().textures)
		{
			auto& vtex = tex.as<VknTexture>();
			image_views_.emplace_back(*vtex.imageView);
			sz = vec2{ vtex.size };
		}
		if (attachments[AttachmentType::eColor].size() && attachments[AttachmentType::eDepth].size())
		{
			rp_type = BasicRenderPasses::eRgbaColorDepth;
		}
		else if(attachments[AttachmentType::eColor].size())
		{
			rp_type = BasicRenderPasses::eRgbaColorOnly;
		}
		else
		{
			rp_type = BasicRenderPasses::eDepthOnly;
		}
		auto& view = View();
		AttachImageViews(view.BasicRenderPass(rp_type), image_views_, view, sz);
	}


	/*GLuint VknFrameBuffer::DepthBuffer() const
	{
		return depthbuffer;
	}*/

}