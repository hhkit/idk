#include "pch.h"
#include "VknFrameBuffer.h"
#include <core/Core.h>
#include <vkn/VknTexture.h>

namespace idk::vkn
{
	VknFrameBuffer::VknFrameBuffer(vector<vk::ImageView>& iv, VulkanView& vknView)
		:buffer{},
		imageViewsRef {iv}
	{
		//Creates an empty framebuffer tht does nothing at all until attach image views is called
		AttachImageViews(iv,vknView);
	}

	VknFrameBuffer::VknFrameBuffer(VknFrameBuffer&& rhs)
		:buffer{std::move(rhs.buffer)},
		imageViewsRef{ std::move(rhs.imageViewsRef) },
		uncreated{std::move(rhs.uncreated)}
	{}

	VknFrameBuffer& VknFrameBuffer::operator=(VknFrameBuffer&& rhs)
	{
		// TODO: insert return statement here
		std::swap(buffer,rhs.buffer);
		std::swap(imageViewsRef,rhs.imageViewsRef);

		return *this;
	}

	VknFrameBuffer::~VknFrameBuffer()
	{
		for (auto& elem : meta.textures)
			Core::GetResourceManager().Free(elem);
		
		//glDeleteRenderbuffers(1, &depthbuffer);
		buffer.reset();
		imageViewsRef.clear();
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

	void VknFrameBuffer::AttachImageViews(vector<vk::ImageView>& iv, VulkanView& vknView)
	{
		//One framebuffer can reference multiple attachments (color, position, light etc.)

		//We are using 3 swapchain images to represent colour attachment

		//Need create other kinds of attachment too in the renderpasses

		//for now vector iv should only have one colour attachment image
		/*vector<vk::ImageView> ref;
		for (auto& elem : iv)
			ref.emplace_back(*elem);*/

		vk::FramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.renderPass = *vknView.Renderpass();
		framebufferInfo.attachmentCount = hlp::arr_count(iv);
		framebufferInfo.pAttachments = std::data(iv);
		framebufferInfo.width = vknView.Swapchain().extent.width;
		framebufferInfo.height = vknView.Swapchain().extent.height;
		framebufferInfo.layers = 1;

		buffer = vknView.Device()->createFramebufferUnique(framebufferInfo,nullptr,vknView.Dispatcher());

		uncreated = false;
	}

	/*GLuint VknFrameBuffer::DepthBuffer() const
	{
		return depthbuffer;
	}*/

}