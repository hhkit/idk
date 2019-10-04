#pragma once
#include <vulkan/vulkan.hpp>
#include <vkn/MemoryAllocator.h>
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



	class bbucket_list //More like a pool ain't it?
	{
		struct bucket;
	public:
		// constructor
		bbucket_list() = default;
		bbucket_list(vk::PhysicalDevice pdevice, vk::Device device, std::shared_ptr<MemoryAllocator> allocator, std::optional<size_t> num_bytes_per_chunk={}) :_pdevice{ pdevice }, _device{ device }, _allocator{ allocator }, _chunk_size{ num_bytes_per_chunk? *num_bytes_per_chunk :_dchunk_size}{}
		bbucket_list(vk::PhysicalDevice pdevice, vk::Device device, MemoryAllocator& allocator, std::optional<size_t> num_bytes_per_chunk={}) :
			_pdevice{ pdevice }, _device{ device }, _allocator{ &allocator,[](MemoryAllocator*) {} }, _chunk_size{ num_bytes_per_chunk ? *num_bytes_per_chunk : _dchunk_size }{}

		// accessor
		bool empty()const { return _buckets.size() == 0 || (_index == 0 && _buckets[_index].size == 0); }
		size_t chunk_size()const { return _chunk_size; }
		bool will_grow(string_view chunk)const;
		template<typename T>
		bool will_grow(T* data, size_t buffer_size)const {
			return will_grow(string_view{ r_cast<const char*>(data), buffer_size });
		}
		// modifiers
		template<typename T>
		std::pair<vk::Buffer, uint32_t > new_chunk(T* data, size_t buffer_size) 
		{
			return new_chunk(string_view{ r_cast<const char*>(data), buffer_size });
		}
		std::pair<vk::Buffer, uint32_t > new_chunk(string_view chunk);
		vk::Buffer current_buffer()const;
		void clear()
		{
			for (auto& bucket : _buckets)
			{
				bucket.clear();
			}
		}
		void update_buffers();
		
	private:
		struct bucket
		{
			size_t capacity{};
			size_t size{};
			UniqueAlloc memory{};
			vk::UniqueBuffer       buffer{};
			string data;
			bucket(vk::Device device, vk::PhysicalDevice pd, MemoryAllocator& allocator, vk::BufferUsageFlags buffer_flags, vk::MemoryPropertyFlags mem_flags, size_t cap);
			bool full()const { return size == capacity; }
			bool full(size_t load)const { return size + load > capacity; }
			uint32_t allocate(string_view d);
			void clear()
			{
				size = 0;
				data.resize(0);
			}
		};

		bucket& back()
		{
			return _buckets[_index];
		}
		const bucket& back()const
		{
			return _buckets[_index];
		}
		void add_bucket();

		vector<bucket>  _buckets;
		shared_ptr<MemoryAllocator> _allocator;

		vk::Device _device;
		vk::PhysicalDevice _pdevice;
		
		vk::UniqueDeviceMemory   _memory;
		vk::UniqueBuffer         _buffer;
		vk::BufferUsageFlags     _usage_flags = vk::BufferUsageFlagBits::eVertexBuffer| vk::BufferUsageFlagBits::eIndexBuffer| vk::BufferUsageFlagBits::eTransferDst;
		vk::MemoryPropertyFlags  _mempr_flags = vk::MemoryPropertyFlagBits::eHostVisible| vk::MemoryPropertyFlagBits::eHostCoherent;
		size_t                   _index = 0;
		size_t                   _capacity = 0;
		static constexpr size_t  _dchunk_size = (1 << 16) - 1;
		const size_t  _chunk_size = _dchunk_size;
	};

}