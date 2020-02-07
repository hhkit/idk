#pragma once
#include <gfx/RenderTarget.h>
//#include <glad/glad.h>
#include <vkn/VulkanState.h>
#include <vkn/VknImageData.h>
#include <vkn/VulkanView.h>
#include <vkn/ManagedVulkanObjs.h>

#include <vkn/VknFrameBuffer.h>

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

		void PrepareDraw(vk::CommandBuffer& cmd_buffer);

		RenderPassObj GetRenderPass(bool clear_col=true,bool clear_depth =true)const;

		vk::Framebuffer Buffer();

		vk::Semaphore ReadySignal();

		//const ivec2& Size()const { return size; };
	private:
		void OnFinalize() override;
		static constexpr BasicRenderPasses     rp_type = BasicRenderPasses::eRgbaColorDepth;
		UniqueSemaphore           ready_semaphore{};
		RscHandle<FrameBuffer> _frame_buffer;
		vector<RscHandle<FrameBuffer>> _permutations;
		//ivec2				      size{};
		bool				      uncreated{true};
	};
}