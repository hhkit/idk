#pragma once
#include <gfx/RenderTarget.h>
//#include <glad/glad.h>
#include <vkn/VulkanState.h>
namespace idk::vkn
{
	class VknFrameBuffer
		: public RenderTarget
	{
	public:
		VknFrameBuffer() = default;
		VknFrameBuffer(vector<vk::ImageView>& iv, VulkanView& vknView);
		VknFrameBuffer(VknFrameBuffer&&);
		VknFrameBuffer& operator=(VknFrameBuffer&&);
		~VknFrameBuffer();

		void OnMetaUpdate(const Metadata& newmeta) override;
		void AttachImageViews(vector<vk::ImageView>&, VulkanView& vknView);

		//GLuint DepthBuffer() const;
	private:
		//GLuint depthbuffer = 0;
		vk::UniqueFramebuffer buffer;
		vector<vk::ImageView> imageViewsRef;
		bool uncreated{true};
	};
}