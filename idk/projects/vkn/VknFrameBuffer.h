#pragma once
#include <gfx/RenderTarget.h>
//#include <glad/glad.h>
#include <vkn/VulkanState.h>
#include <vkn/VknImageData.h>
namespace idk::vkn
{
	class VknFrameBuffer
		: public RenderTarget
	{
	public:
		VknFrameBuffer() = default;
		VknFrameBuffer(VknImageData iv, VulkanView& vknView);
		VknFrameBuffer(VknFrameBuffer&&);
		VknFrameBuffer& operator=(VknFrameBuffer&&);
		~VknFrameBuffer();

		void OnMetaUpdate(const Metadata& newmeta) override;
		void AttachImageViews(VknImageData&, VulkanView& vknView);

		vk::UniqueFramebuffer Buffer();

		//GLuint DepthBuffer() const;
	private:
		//GLuint depthbuffer = 0;
		vk::UniqueFramebuffer buffer{ nullptr };
		VknImageData		  image{};
		bool				  uncreated{true};
	};
}