#include "pch.h"
#include "VulkanView.h"
#include <vkn/VulkanState.h>
#include <vkn/VulkanHelpers.h>
#include <vkn/VectorBuffer.h>
#include <vkn/RenderState.h>

namespace idk
{
	namespace vgfx
	{
		struct VulkanView::pimpl
		{
			vgfx::RenderState& PrevRenderState() { return render_state_[(index_ + max() - 1) % max()]; }
			vgfx::RenderState& CurrentRenderState() { return render_state_[index_]; }
			vector<vgfx::RenderState>& RenderStates() { return render_state_; };
			void SwapRenderState() { index_ = s_cast<uint32_t>((index_ + 1) % max()); }
		private:
			uint32_t max()const { return s_cast<uint32_t>(render_state_.size()); }
			vector<vgfx::RenderState> render_state_{};
			uint32_t index_{};
		};
		vk::DispatchLoaderDefault& VulkanView::Dispatcher() const { return vulkan().dispatcher; }

		vk::DispatchLoaderDynamic& VulkanView::DynDispatcher()const { return vulkan().dyn_dispatcher; }
		vk::UniqueInstance& VulkanView::Instance()const { return vulkan().instance; }
		vk::UniqueSurfaceKHR& VulkanView::Surface()const { return vulkan().m_surface; }
		vk::PhysicalDevice& VulkanView::PDevice()const { return vulkan().pdevice; }
		vk::UniqueDevice& VulkanView::Device()const { return vulkan().m_device; }
		QueueFamilyIndices& VulkanView::QueueFamily()const { return vulkan().m_queue_family; }
		vk::Queue& VulkanView::GraphicsQueue()const { return vulkan().m_graphics_queue; }
		vk::Queue& VulkanView::PresentQueue()const { return vulkan().m_present_queue; }
		//vk::Queue          m_transfer_queue = {}{}					                  				 ;
		SwapChainInfo& VulkanView::Swapchain()const { return vulkan().m_swapchain; }

		void VulkanView::SwapRenderState() const
		{
			return impl_->SwapRenderState();
		}

		idk::vector<RenderState>& VulkanView::RenderStates() const
		{
			// TODO: insert return statement here
			return impl_->RenderStates();
		}

		RenderState& VulkanView::PrevRenderState() const
		{
			// TODO: insert return statement here
			return impl_->PrevRenderState();
		}

		RenderState& VulkanView::CurrRenderState() const
		{
			// TODO: insert return statement here
			return impl_->CurrentRenderState();
		}

		vk::UniqueRenderPass& VulkanView::Renderpass()const { return vulkan().m_renderpass; }
		vk::UniquePipeline& VulkanView::Pipeline()const { return vulkan().m_pipeline; }
		vk::UniqueCommandPool& VulkanView::Commandpool()const { return vulkan().m_commandpool; }

		vk::UniqueCommandBuffer& VulkanView::CurrCommandbuffer()const { return vulkan().m_commandbuffers[vulkan().current_frame]; }
		vk::Buffer& VulkanView::CurrMasterVtxBuffer() const
		{
			return impl_->CurrentRenderState().MasterBuffer().host_buffer.Buffer();
		}
		uint32_t VulkanView::AddToMasterBuffer(const void* data, uint32_t len) const
		{
			return impl_->CurrentRenderState().MasterBuffer().Add(data, len);
		}
		void VulkanView::ResetMasterBuffer() const
		{
			impl_->CurrentRenderState().MasterBuffer().Reset();
		}
		std::vector<vk::UniqueCommandBuffer>& VulkanView::Commandbuffers()const { return vulkan().m_commandbuffers; }

		vk::UniqueShaderModule VulkanView::CreateShaderModule(const idk::string_view& code)
		{
			vk::ShaderModuleCreateInfo mod{
				vk::ShaderModuleCreateFlags{},
				code.length(),reinterpret_cast<uint32_t const*>(code.data())
			};
			return Device()->createShaderModuleUnique(mod);
		}

		VulkanView::VulkanView(idk::VulkanState& vulkan) :vulkan_{ &vulkan }, impl_{ std::make_unique<pimpl>() } {}
		VulkanView::VulkanView(VulkanView&&) noexcept = default;
		VulkanView& VulkanView::operator=(VulkanView&&) noexcept = default;
		idk::VulkanState& VulkanView::vulkan()const
		{
			return *vulkan_;
		}

		VulkanView::~VulkanView()
		{
			impl_.reset();
		}
	}
}