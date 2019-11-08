#include "pch.h"
#include "vector_buffer.h"
#include <vkn/BufferHelpers.h>
#include <vkn/VulkanView.h>
#undef min
#undef max
namespace idk::vkn
{

	VulkanView& View();
	namespace hlp {
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
		void vector_buffer::resize(size_t num_bytes, bool force_downsize)
		{
			vk::PhysicalDevice& pdevice = View().PDevice();
			vk::Device& device = *View().Device();
			resize(pdevice, device, num_bytes, force_downsize);
		}
		void vector_buffer::update(vk::DeviceSize offset, vk::DeviceSize len, vk::CommandBuffer& cmd_buffer, unsigned char const* data)
		{
			if (offset + len > _capacity)
			{
				LOG_TO(LogPool::GFX, "Failed to update vector buffer at [Offset:%llu] with data of [Length:%llu], exceeding [Capacity:%llu]", offset, len, _capacity);
				return;
			}
			constexpr size_t limit = 65536;
			size_t limit_offset = 0;
			do {
				auto limit_len = std::min(len-limit_offset, limit);
				cmd_buffer.updateBuffer(*_buffer, limit_offset+offset, hlp::make_array_proxy(static_cast<uint32_t>(limit_len), data), vk::DispatchLoaderDefault{});
				limit_offset += limit_len;
			} while (limit_offset<len);
		}


		// modifiers

		bool bbucket_list::will_grow(string_view chunk) const
		{
			return empty() || back().full(chunk.size());
		}

		std::pair<vk::Buffer, uint32_t> bbucket_list::new_chunk(string_view chunk)
		{
			if (_buckets.size() == 0 || back().full(chunk.size()))
			{
				add_bucket();
			}
			auto& bucket = _buckets.back();
			return std::make_pair(*bucket.buffer, bucket.allocate(chunk));
		}

		vk::Buffer bbucket_list::current_buffer() const { return *back().buffer; }

		void bbucket_list::update_buffers()
		{
			if (!empty())
				for (size_t i = 0; i < _index + 1; i++)
				{
					auto& b = _buckets[i];
					auto data = std::data(b.data);
					if (b.size)
						hlp::MapMemory(_device, b.memory->Memory(), b.memory->Offset(), data, b.size);
				}
		}
		void bbucket_list::add_bucket()
		{
			if (!empty())
				++_index;
			if (_index >= _buckets.size())
			{
				_buckets.emplace_back(_device, _pdevice, *_allocator, _usage_flags, _mempr_flags, _chunk_size);
			}
		}
		bbucket_list::bucket::bucket(vk::Device device, vk::PhysicalDevice pd, MemoryAllocator& allocator, vk::BufferUsageFlags buffer_flags, vk::MemoryPropertyFlags mem_flags, size_t cap) :capacity{ cap }, data(cap, '0')
		{
			auto&& [buf, alloc] = hlp::CreateAllocBindBuffer(pd, device, cap, buffer_flags, mem_flags, allocator, vk::DispatchLoaderDefault{});
			buffer = std::move(buf);
			memory = std::move(alloc);
			data.resize(0);
		}
		uint32_t bbucket_list::bucket::allocate(string_view d)
		{
			uint32_t new_size = s_cast<uint32_t>(size);
			size += d.size();
			data += d;
			return new_size;
		}

	}
}