#include "pch.h"
#include "VulkanMesh.h"
#include <gfx/CompiledMesh.h>
#include <vkn/VknMeshModder.h>
#include <vkn/BufferHelpers.h>
#include <vkn/VulkanMeshFactory.h>
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>
#include <vkn/BufferHelpers.inl>
namespace idk::vkn
{
	VulkanMesh::VulkanMesh(const CompiledMesh& m)
	{
		bounding_volume = m.bounding_volume;
		auto& mm = Core::GetResourceManager().GetFactory<MeshFactory>().mesh_modder;


		using offset_t = size_t;
		hash_table<attrib_index, std::pair<std::shared_ptr<MeshBuffer::Managed>, offset_t>> attribs;
		for (auto& compiled_buffer : m.buffers)
		{
			auto buffer = mm.CreateBuffer(string_view{ r_cast<const char*>(compiled_buffer.data.data()), hlp::buffer_size(compiled_buffer.data) });
			auto& attrib = compiled_buffer.attribs;
			for (auto& att : attrib)
			{
				
				attribs[att.attrib] = std::make_pair(buffer, offset_t{ att.offset });
			}
		}
		mm.RegisterAttribs(*this, attribs);
		mm.SetIndexBuffer32(*this, mm.CreateBuffer(string_view{ r_cast<const char*>(std::data(m.element_buffer)),hlp::buffer_size(m.element_buffer) }),s_cast<uint32_t>(m.element_buffer.size()));
	}
	//// 
	const MeshBuffer& VulkanMesh::Get(attrib_index index) const
	{
		auto itr = buffers.find(index);
		if (itr != buffers.end())
			return itr->second;
		throw std::exception{ "Attempting to get invalid attrib index from vulkan mesh." };
		//return MeshBuffer{};
	}
	bool VulkanMesh::Has(attrib_index index) const
	{
		auto itr = buffers.find(index);
		return (itr != buffers.end());
	}
	int VulkanMesh::GetAttribs() const
{
	//TODO Actually get the attribs
	return 0;
}

void VulkanMesh::SetIndexBuffer(MeshBuffer&& buffer, uint32_t count, vk::IndexType type) { index_buffer = std::move(buffer); index_count = count; index_type = type; }

void VulkanMesh::SetBuffer(attrib_index type, MeshBuffer&& buffer)
{
	buffers[type] = std::move(buffer);
}

}