#include "pch.h"
#include "vector_buffer.h"
#include <vkn/BufferHelpers.h>

namespace idk::vkn::hlp
{
	namespace detail
	{
		static size_t grow_size(size_t sz)
		{
			size_t new_size = 1;
			while (new_size < sz)
			{
				new_size *= 2;
			}
			return new_size;
		}
	}

	vector_buffer::vector_buffer(vk::PhysicalDevice& pdevice, vk::Device& device, size_t num_bytes)
	{
		resize(pdevice, device, num_bytes);
	}


	void vector_buffer::resize(vk::PhysicalDevice& pdevice, vk::Device& device, size_t num_bytes, bool force_downsize)
	{
		if (num_bytes >= _capacity || force_downsize)
		{
			_capacity = detail::grow_size(num_bytes);

			auto [buf, mem] = hlp::CreateAllocBindBuffer(
				pdevice, device, _capacity,
				vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
				vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible,
				vk::DispatchLoaderDefault{}
			);

			_memory = std::move(mem);
			_buffer = std::move(buf);
		}
	}
	void vector_buffer::update(vk::DeviceSize offset, vk::DeviceSize len, vk::CommandBuffer& cmd_buffer, unsigned char const* data)
	{
		cmd_buffer.updateBuffer(*_buffer, offset, hlp::make_array_proxy(static_cast<uint32_t>(len), data), vk::DispatchLoaderDefault{});
	}

}