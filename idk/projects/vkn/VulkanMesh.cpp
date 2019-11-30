#include "pch.h"
#include "VulkanMesh.h"
namespace idk::vkn
{
//#pragma optimize("",off)
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