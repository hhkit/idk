#pragma once
#include <vulkan/vulkan.hpp>

namespace idk::vkn::hlp
{
	// buffer that acts like a vector
	class vector_buffer
	{
	public:
		// constructor
		vector_buffer() = default;
		vector_buffer(vk::PhysicalDevice& pdevice, vk::Device& device, size_t num_bytes);

		// accessor
		vk::Buffer& buffer() { return *_buffer; }

		// modifiers
		void resize(vk::PhysicalDevice& pdevice, vk::Device& device, size_t num_bytes, bool force_downsize = false);
		void update(vk::DeviceSize offset, vk::DeviceSize len, vk::CommandBuffer& buffer, const unsigned char * data); 
		
		template<typename T>
		void update(vk::DeviceSize offset, vk::ArrayProxy<T> arr, vk::CommandBuffer& buffer) { update(offset, hlp::buffer_size(arr), buffer, reinterpret_cast<unsigned char const*>(arr.data())); }

	private:
		vk::UniqueDeviceMemory _memory;
		vk::UniqueBuffer       _buffer;
		size_t                 _capacity = 0;
	};
}