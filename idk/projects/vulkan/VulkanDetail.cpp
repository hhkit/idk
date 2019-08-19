#include "pch.h"
#include "VulkanDetail.h"
#include <vulkan/vkn.h>
#include <VulkanHelpers.h>
#include <VectorBuffer.h>
#include <RenderState.h>

namespace idk
{
	namespace vgfx
	{
		struct VulkanDetail::pimpl
		{
			vgfx::RenderState& PrevRenderState() { return render_state_[(index_ + max() - 1) % max()]; }
			vgfx::RenderState& CurrentRenderState() { return render_state_[index_]; }
			idk::vector<vgfx::RenderState>& RenderStates() { return render_state_; };
			void SwapRenderState() { index_ = idk::s_cast<uint32_t>((index_ + 1) % max()); }
		private:
			uint32_t max()const { return idk::s_cast<uint32_t>(render_state_.size()); }
			idk::vector<vgfx::RenderState> render_state_{};
			uint32_t index_{};
		};
		vk::DispatchLoaderDefault& VulkanDetail::Dispatcher() const { return vulkan().dispatcher; }

		vk::DispatchLoaderDynamic& VulkanDetail::DynDispatcher()const { return vulkan().dyn_dispatcher; }
		vk::UniqueInstance& VulkanDetail::Instance()const { return vulkan().instance; }
		vk::UniqueSurfaceKHR& VulkanDetail::Surface()const { return vulkan().m_surface; }
		vk::PhysicalDevice& VulkanDetail::PDevice()const { return vulkan().pdevice; }
		vk::UniqueDevice& VulkanDetail::Device()const { return vulkan().m_device; }
		QueueFamilyIndices& VulkanDetail::QueueFamily()const { return vulkan().m_queue_family; }
		vk::Queue& VulkanDetail::GraphicsQueue()const { return vulkan().m_graphics_queue; }
		vk::Queue& VulkanDetail::PresentQueue()const { return vulkan().m_present_queue; }
		//vk::Queue          m_transfer_queue = {}{}					                  				 ;
		SwapChainInfo& VulkanDetail::Swapchain()const { return vulkan().m_swapchain; }

		void VulkanDetail::SwapRenderState() const
		{
			return impl_->SwapRenderState();
		}

		idk::vector<RenderState>& VulkanDetail::RenderStates() const
		{
			// TODO: insert return statement here
			return impl_->RenderStates();
		}

		RenderState& VulkanDetail::PrevRenderState() const
		{
			// TODO: insert return statement here
			return impl_->PrevRenderState();
		}

		RenderState& VulkanDetail::CurrRenderState() const
		{
			// TODO: insert return statement here
			return impl_->CurrentRenderState();
		}

		vk::UniqueRenderPass& VulkanDetail::Renderpass()const { return vulkan().m_renderpass; }
		vk::UniquePipeline& VulkanDetail::Pipeline()const { return vulkan().m_pipeline; }
		vk::UniqueCommandPool& VulkanDetail::Commandpool()const { return vulkan().m_commandpool; }

		vk::UniqueCommandBuffer& VulkanDetail::CurrCommandbuffer()const { return vulkan().m_commandbuffers[vulkan().current_frame]; }
		vk::Buffer& VulkanDetail::CurrMasterVtxBuffer() const
		{
			return impl_->CurrentRenderState().MasterBuffer().host_buffer.Buffer();
		}
		uint32_t VulkanDetail::AddToMasterBuffer(const void* data, uint32_t len) const
		{
			return impl_->CurrentRenderState().MasterBuffer().Add(data, len);
		}
		void VulkanDetail::ResetMasterBuffer() const
		{
			impl_->CurrentRenderState().MasterBuffer().Reset();
		}
		std::vector<vk::UniqueCommandBuffer>& VulkanDetail::Commandbuffers()const { return vulkan().m_commandbuffers; }

		vk::UniqueShaderModule VulkanDetail::CreateShaderModule(const idk::string_view& code)
		{
			vk::ShaderModuleCreateInfo mod{
				vk::ShaderModuleCreateFlags{},
				code.length(),reinterpret_cast<uint32_t const*>(code.data())
			};
			return Device()->createShaderModuleUnique(mod);
		}

		VulkanDetail::VulkanDetail(idk::Vulkan& vulkan) :vulkan_{ &vulkan }, impl_{ std::make_unique<pimpl>() } {}
		VulkanDetail::VulkanDetail(VulkanDetail&&) noexcept = default;
		VulkanDetail& VulkanDetail::operator=(VulkanDetail&&) noexcept = default;
		idk::Vulkan& VulkanDetail::vulkan()const
		{
			return *vulkan_;
		}

		VulkanDetail::~VulkanDetail()
		{
			impl_.reset();
		}
	}
}