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
		VknFrameBuffer(vk::UniqueImage img, vector<vk::ImageView> iv, VulkanView& vknView, vec2 fbsize = {1,1});
		VknFrameBuffer(VknFrameBuffer&&);
		VknFrameBuffer& operator=(VknFrameBuffer&&);
		~VknFrameBuffer();

		void OnMetaUpdate(const Metadata& newmeta) override;
		void AttachImageViews(VknImageData&, VulkanView& vknView);
		void AttachImageViews(vk::UniqueImage img, vector<vk::ImageView> iv, VulkanView& vknView, vec2 size = {});

		vk::UniqueFramebuffer Buffer();

		//GLuint DepthBuffer() const;
	private:
		//GLuint depthbuffer = 0;
		vk::UniqueFramebuffer buffer{ nullptr };
		//VknImageData		  image{};
		vk::UniqueImage       image{nullptr};
		
		vector<vk::ImageView> imageView{};
		vec2				  size{};
		bool				  uncreated{true};
	};
}