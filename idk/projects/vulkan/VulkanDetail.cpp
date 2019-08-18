#include "pch.h"
#include "VulkanDetail.h"
#include "Vulkan.h"
#include <VulkanHelpers.h>

namespace vgfx
{
	class VectorBuffer
	{
	public:
		VectorBuffer()=default;
		VectorBuffer(vk::PhysicalDevice& pdevice, vk::Device& device, size_t num_bytes);
		void Resize(vk::PhysicalDevice& pdevice, vk::Device& device, size_t num_bytes, bool force_downsize = false);
		template<typename T>
		void Update(vk::DeviceSize offset, vk::ArrayProxy<T> arr, vk::CommandBuffer& buffer)
		{
			Update(offset, vhlp::buffer_size(arr), buffer, reinterpret_cast<unsigned char const*>(arr.data()));
		}
		void Update(vk::DeviceSize offset, vk::DeviceSize len, vk::CommandBuffer& buffer, unsigned char const* data);
		vk::Buffer       &Buffer() { return *buffer; }
	private:
		vk::UniqueDeviceMemory memory;
		vk::UniqueBuffer       buffer;

		size_t capacity = 0;

	};

	struct VulkanDetail::pimpl
	{
		struct RenderState
		{
			struct MasterBuffer
			{
				idk::string             buffer{};
				//vk::UniqueBuffer        device_buffer;
				VectorBuffer            host_buffer{};

				uint32_t Add(const void* data, size_t len)
				{
					uint32_t start_index = idk::s_cast<uint32_t>(buffer.size());
					const char* ptr = idk::s_cast<const char*>(data);
					buffer.append(ptr, len);
					return start_index;
				}
				void Reset()
				{
					buffer.resize(0);
				}
				void UpdateBuffer(VulkanDetail& detail, RenderState& state)
				{
					host_buffer.Resize(detail.PDevice(), *detail.Device(), buffer.size());
					host_buffer.Update(0, vhlp::make_array_proxy(idk::s_cast<uint32_t>(std::size(buffer)), std::data(buffer)), state.TransferBuffer());
				}
			};
			MasterBuffer            master_buffer  {};
			vk::UniqueRenderPass    render_pass    {};
			vk::UniqueCommandBuffer	command_buffer {};
			vk::UniqueCommandBuffer	transfer_buffer{};

			MasterBuffer        &MasterBuffer  (){return master_buffer  ;}
			vk::RenderPass      &RenderPass    (){return *render_pass    ;}
			vk::CommandBuffer	&TransferBuffer(){return *transfer_buffer;}
			vk::CommandBuffer	&CommandBuffer (){return *command_buffer ;}

		};

		RenderState& CurrentRenderState() { return render_state_; }
	private:
		RenderState render_state_{};
	};
    vk::DispatchLoaderDefault& VulkanDetail::Dispatcher() const                     { return vulkan().dispatcher      ;}
																												     
	vk::DispatchLoaderDynamic& VulkanDetail::DynDispatcher()const                   { return vulkan().dyn_dispatcher  ;}
	vk::UniqueInstance&        VulkanDetail::Instance()const                        { return vulkan().instance        ;}
	vk::UniqueSurfaceKHR             &VulkanDetail::Surface()const                  { return vulkan().m_surface       ;}
	vk::PhysicalDevice               &VulkanDetail::PDevice()const                  { return vulkan().pdevice         ;}
	vk::UniqueDevice                 &VulkanDetail::Device()const                   { return vulkan().m_device        ;}
	QueueFamilyIndices& VulkanDetail::QueueFamily()const                            { return vulkan().m_queue_family  ;}
	vk::Queue&          VulkanDetail::GraphicsQueue()const                          { return vulkan().m_graphics_queue;}
	vk::Queue&          VulkanDetail::PresentQueue()const                           { return vulkan().m_present_queue ;}
	//vk::Queue          m_transfer_queue = {}{}					                  				 ;
	SwapChainInfo                    & VulkanDetail::Swapchain()const               { return vulkan().m_swapchain     ;}
																				      				 			     
	vk::UniqueRenderPass                 &VulkanDetail::Renderpass         ()const  { return vulkan().m_renderpass    ;}
	vk::UniquePipeline                   &VulkanDetail::Pipeline           ()const  { return vulkan().m_pipeline      ;}
	vk::UniqueCommandPool                &VulkanDetail::Commandpool        ()const  { return vulkan().m_commandpool   ;}
																					  				 
	vk::UniqueCommandBuffer              &VulkanDetail::CurrCommandbuffer  ()const  { return vulkan().m_commandbuffers[vulkan().current_frame];}
	vk::Buffer& VulkanDetail::CurrMasterVtxBuffer() const
	{
		// TODO: insert return statement here
		return impl_->CurrentRenderState().MasterBuffer().host_buffer.Buffer();
	}
	uint32_t VulkanDetail::AddToMasterBuffer(const void* data, uint32_t len) const
	{
		// TODO: insert return statement here
		return impl_->CurrentRenderState().MasterBuffer().Add(data, len);
	}
	void VulkanDetail::ResetMasterBuffer() const
	{
		impl_->CurrentRenderState().MasterBuffer().Reset();
	}
	std::vector<vk::UniqueCommandBuffer> &VulkanDetail::Commandbuffers     ()const  { return vulkan().m_commandbuffers;}

	inline vk::UniqueShaderModule VulkanDetail::CreateShaderModule(const idk::string_view& code)
	{
		vk::ShaderModuleCreateInfo mod{
			vk::ShaderModuleCreateFlags{},
			code.length(),reinterpret_cast<uint32_t const*>(code.data())
		};
		return Device()->createShaderModuleUnique(mod);
	}

	VulkanDetail::VulkanDetail(::Vulkan& vulkan) :vulkan_{ &vulkan }, impl_{ std::make_unique<pimpl>() } {}
	VulkanDetail::VulkanDetail(VulkanDetail&&) noexcept = default;
	VulkanDetail& VulkanDetail::operator=(VulkanDetail&&) noexcept = default;
	::Vulkan& VulkanDetail::vulkan()const
	{
		return *vulkan_;
	}

	VectorBuffer::VectorBuffer(vk::PhysicalDevice& pdevice, vk::Device& device, size_t num_bytes)
	{
		Resize(pdevice, device, num_bytes);
	}

	size_t grow_size(size_t sz)
	{
		size_t new_size = 1;
		while (new_size < sz)
		{
			new_size <<= 1;
		}
		return new_size;
	}

	void VectorBuffer::Resize(vk::PhysicalDevice& pdevice, vk::Device& device, size_t num_bytes, bool force_downsize)
	{
		auto dispatcher = vk::DispatchLoaderDefault{};
		if (num_bytes >= capacity || force_downsize)
		{
			capacity = grow_size(num_bytes);
			if (buffer)
			{
				
				auto tmp_mem = vhlp::AllocateBuffer(pdevice, device, *buffer, vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible, dispatcher);
				vhlp::BindBufferMemory(device, *buffer, *tmp_mem, 0, dispatcher);
				memory = std::move(tmp_mem);
			}
			else
			{
				auto [buf, mem] = vhlp::CreateAllocBindBuffer(
					pdevice, device, num_bytes,
					vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
					vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible,
					dispatcher
					);
				memory = std::move(mem);
				buffer = std::move(buf);
			}
		}
	}
	void VectorBuffer::Update(vk::DeviceSize offset, vk::DeviceSize len, vk::CommandBuffer& cmd_buffer, unsigned char const* data)
	{
		cmd_buffer.updateBuffer(*buffer, offset, vhlp::make_array_proxy(static_cast<uint32_t>(len), data), vk::DispatchLoaderDefault{});
	}
	VulkanDetail::~VulkanDetail() 
	{
		impl_.reset();
	}
}