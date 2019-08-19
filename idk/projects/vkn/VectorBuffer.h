#pragma once
#include <vulkan/vulkan.hpp>

namespace idk
{
	namespace vgfx
	{
		class VectorBuffer
		{
		public:
			VectorBuffer() = default;
			VectorBuffer(vk::PhysicalDevice& pdevice, vk::Device& device, size_t num_bytes);
			void Resize(vk::PhysicalDevice& pdevice, vk::Device& device, size_t num_bytes, bool force_downsize = false);
			template<typename T>
			void Update(vk::DeviceSize offset, vk::ArrayProxy<T> arr, vk::CommandBuffer& buffer) { Update(offset, vhlp::buffer_size(arr), buffer, reinterpret_cast<unsigned char const*>(arr.data())); }
			void Update(vk::DeviceSize offset, vk::DeviceSize len, vk::CommandBuffer& buffer, unsigned char const* data);
			vk::Buffer& Buffer() { return *buffer; }
		private:
			vk::UniqueDeviceMemory memory;
			vk::UniqueBuffer       buffer;

			size_t capacity = 0;
		};
	}
}