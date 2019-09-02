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
		const MeshBuffer& Get(attrib_index index)const;
		const buffers_t& Buffers()const { return buffers; }
		int GetAttribs() const override;
		const std::optional<MeshBuffer>& GetIndexBuffer()const { return index_buffer; }
		uint32_t IndexCount()const { return index_count; }
		void SetIndexBuffer(MeshBuffer&& buffer, uint32_t count, vk::IndexType type);
		void SetBuffer(attrib_index type, MeshBuffer&& buffer);
		vk::IndexType IndexType()const { return index_type; }
	private:
		buffers_t buffers{};
		vk::IndexType index_type = vk::IndexType::eUint16;
		uint32_t index_count;
		std::optional<MeshBuffer> index_buffer;
	};

}
