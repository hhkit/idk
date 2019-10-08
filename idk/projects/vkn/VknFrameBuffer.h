#pragma once
#include <gfx/RenderTarget.h>
//#include <glad/glad.h>
#include <vkn/VulkanState.h>
#include <vkn/VknImageData.h>
#include <vkn/VulkanView.h>
namespace idk::vkn
{
	class VknRenderTarget
		: public RenderTarget
	{
	public:
		VknRenderTarget() = default;
		VknRenderTarget(VknRenderTarget&&) noexcept= default;
		VknRenderTarget& operator=(VknRenderTarget&&) noexcept = default;
		VknRenderTarget(const VknRenderTarget&) = delete;
		VknRenderTarget& operator=(const VknRenderTarget&) = delete;
		~VknRenderTarget() = default;

		void OnMetaUpdate(const Metadata& newmeta) override;
		void PrepareDraw(vk::CommandBuffer& cmd_buffer);

		vk::RenderPass GetRenderPass()const;

		vk::Framebuffer Buffer();

		vk::Semaphore ReadySignal();

		const ivec2& Size()const { return size; };
		BasicRenderPasses GetRenderPassType() { return rp_type; }
	private:
		BasicRenderPasses     rp_type = BasicRenderPasses::eRgbaColorDepth;
		vk::UniqueSemaphore   ready_semaphore;
		vk::UniqueFramebuffer buffer{ nullptr };
		ivec2				  size{};
		bool				  uncreated{true};
	};
}