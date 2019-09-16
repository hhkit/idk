#include "pch.h"
#include "VknFrameBuffer.h"
#include <core/Core.h>
#include <vkn/VknTexture.h>

namespace idk::vkn
{
	VknFrameBuffer::VknFrameBuffer(VknTexture iv, VulkanView& vknView)
		:buffer{},
		image{std::move(iv.image)},
		//imageView{std::move(iv.imageView)},
		size{iv.size}
	{
		imageView.emplace_back(*iv.imageView);
		//Creates an empty framebuffer tht does nothing at all until attach image views is called

		AttachImageViews(std::move(iv.image), imageView, vknView,size);
	}

	VknFrameBuffer::VknFrameBuffer(unique_ptr<VknTexture> iv, VulkanView& vknView)
		:buffer{},
		image{ std::move(iv->image) },
		//imageView{std::move(iv.imageView)},
		size{ iv->size }
	{
		imageView.emplace_back(std::move(*iv->imageView));
		//Creates an empty framebuffer tht does nothing at all until attach image views is called

		AttachImageViews(std::move(iv->image), imageView, vknView,size);
	}

	VknFrameBuffer::VknFrameBuffer(vk::UniqueImage img, vector<vk::ImageView> iv, VulkanView& vknView, vec2 fbsize)
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

	VknFrameBuffer::VknFrameBuffer(VknFrameBuffer&& rhs)
		:buffer{std::move(rhs.buffer)},
		image{ std::move(rhs.image) },
		imageView{std::move(rhs.imageView)},
		size{ std::move(size) },
		uncreated{std::move(rhs.uncreated)}
	{}

	VknFrameBuffer& VknFrameBuffer::operator=(VknFrameBuffer&& rhs)
	{
		// TODO: insert return statement here
		std::swap(buffer,rhs.buffer);
		std::swap(image,rhs.image);
		std::swap(imageView,rhs.imageView);
		std::swap(size,rhs.size);
		std::swap(uncreated,rhs.uncreated);

		return *this;
	}

	VknFrameBuffer::~VknFrameBuffer()
	{
		for (auto& elem : meta.textures)
			Core::GetResourceManager().Free(elem);
		
		//glDeleteRenderbuffers(1, &depthbuffer);
		buffer.reset();
		imageView.clear();
		image.reset();
	}

	void VknFrameBuffer::OnMetaUpdate(const Metadata& newmeta)
	{
		for (auto& elem : meta.textures)
			Core::GetResourceManager().Free(elem);

		/*glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, newmeta.size.x, newmeta.size.y);*/

		for (auto& elem : newmeta.textures)
		{
			Core::GetResourceManager().Emplace<VknTexture>(elem.guid)->Size(newmeta.size);
		}
	}

	void VknFrameBuffer::ReattachImageViews(VulkanView& vknView)
	{
		buffer.reset();
		vector<vk::ImageView> image_views;
		auto& meta = GetMeta();
		//TODO assert that all textures have to be the same size
		for (auto& im : meta.textures)
		{
			image_views.emplace_back(*im.as<VknTexture>().imageView);
		}

		vk::FramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.renderPass = *vknView.Renderpass();
		framebufferInfo.attachmentCount = image_views.size();
		framebufferInfo.pAttachments = image_views.data();
		framebufferInfo.width  = s_cast<uint32_t>(meta.size.x);
		framebufferInfo.height = s_cast<uint32_t>(meta.size.y);
		framebufferInfo.layers = 1;

		buffer = vknView.Device()->createFramebufferUnique(framebufferInfo, nullptr, vknView.Dispatcher());

		uncreated = false;
	}

	void VknFrameBuffer::AttachImageViews(VknImageData& iv, VulkanView& vknView)
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

		uncreated = false;
	}

	void VknFrameBuffer::AttachImageViews(vk::UniqueImage img, vector<vk::ImageView> iv, VulkanView& vknView, vec2 size)
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
		framebufferInfo.width  = s_cast<uint32_t>(size.x);
		framebufferInfo.height = s_cast<uint32_t>(size.y);
		framebufferInfo.layers = 1;

		buffer = vknView.Device()->createFramebufferUnique(framebufferInfo, nullptr, vknView.Dispatcher());

		uncreated = false;
	}

	vk::Framebuffer VknFrameBuffer::Buffer()
	{
		return *buffer;
	}

	/*GLuint VknFrameBuffer::DepthBuffer() const
	{
		return depthbuffer;
	}*/

}