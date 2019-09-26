#pragma once
#include <gfx/RenderTarget.h>
//#include <glad/glad.h>
#include <vkn/VulkanState.h>
#include <vkn/VknImageData.h>
#include <vkn/VulkanView.h>
namespace idk::vkn
{
	class VknFrameBuffer
		: public RenderTarget
	{
	public:
		VknFrameBuffer() = default;
		VknFrameBuffer(VknTexture iv, VulkanView& vknView); 
		VknFrameBuffer(unique_ptr<VknTexture> iv, VulkanView& vknView);
		VknFrameBuffer(vk::UniqueImage img, vector<vk::ImageView> iv, VulkanView& vknView, vec2 fbsize = {1,1});
		VknFrameBuffer(VknFrameBuffer&&);
		VknFrameBuffer& operator=(VknFrameBuffer&&);
		~VknFrameBuffer();

		void OnMetaUpdate(const Metadata& newmeta) override;
		void ReattachImageViews(VulkanView& vknView);
		void AttachImageViews(VknImageData&, VulkanView& vknView);
		void AttachImageViews(vk::UniqueImage img, vector<vk::ImageView> iv, VulkanView& vknView, vec2 size = {});
		void AttachImageViews(vector<vk::ImageView> iv, VulkanView& vknView, vec2 size = {});
		void PrepareDraw(vk::CommandBuffer& cmd_buffer);

		vk::RenderPass GetRenderPass()const;

		vk::Framebuffer Buffer();

		vk::Semaphore ReadySignal();
		BasicRenderPasses     rp_type;



		void Finalize() override; //Finalizes the framebuffer

		//GLuint DepthBuffer() const;
	private:
		void  AddAttachmentImpl(AttachmentType type, RscHandle<Texture> texture) override;
		vk::UniqueSemaphore   ready_semaphore;
		//GLuint depthbuffer = 0;
		vk::UniqueFramebuffer buffer{ nullptr };
		//VknImageData		  image{};
		vk::UniqueImage       image{nullptr};
		vector<vk::UniqueImageView> image_views;
		vector<vk::ImageView> imageView{};
		vec2				  size{};
		bool				  uncreated{true};
	};
}