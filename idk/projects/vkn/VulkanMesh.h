#pragma once
#include <idk.h>
#include <gfx/Mesh.h>
#include <vulkan/vulkan.hpp>
#include <gfx/vertex_descriptor.h>
#include <vkn/MemoryAllocator.h>

namespace idk::vkn
{
	using attrib_index = vtx::Attrib;
	struct MeshBuffer
	{
		vk::UniqueBuffer buffer{};
		hlp::UniqueAlloc alloc{};
		size_t len{};
	};
	class VulkanMesh : public Mesh
	{
	public:
		using buffers_t = hash_table<attrib_index ,MeshBuffer>;
		const MeshBuffer& Get(attrib_index index)const 
		{
			auto itr = buffers.find(index);
			if(itr!=buffers.end())
				return itr->second;
			throw std::exception{ "Attempting to get invalid attrib index from vulkan mesh." };
			//return MeshBuffer{};
		}
		const buffers_t& Buffers()const { return buffers; }
		int GetAttribs() const override;
		const std::optional<MeshBuffer>& GetIndexBuffer()const { return index_buffer; }
		uint32_t IndexCount()const { return index_count; }
		void SetIndexBuffer(MeshBuffer&& buffer, uint32_t count) { index_buffer = std::move(buffer); index_count = count; }
		void SetBuffer(attrib_index type,MeshBuffer&& buffer)
		{
			buffers[type]=std::move(buffer);
		}
	private:
		buffers_t buffers{};
		uint32_t index_count;
		std::optional<MeshBuffer> index_buffer;
	};

}
