#include "pch.h"
#include "VulkanMeshAsyncLoader.h"
#include "core/Core.inl"
#include "res/ResourceManager.inl"
#include "VulkanMeshFactory.h"
#include "BufferHelpers.inl"
namespace idk::vkn
{

void VulkanMeshAsyncLoader::ProcessEntry(Entry& entry)
{
	auto& m = entry.compiled_mesh;
	auto& factory = Core::GetResourceManager().GetFactory<MeshFactory>();
	auto& mm = factory.mesh_modder;


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
	auto& vmesh = *entry.handle;// handle.as<VulkanMesh>();
	vmesh.bounding_volume = m.bounding_volume;
	mm.RegisterAttribs(vmesh, attribs);
	mm.SetIndexBuffer32(vmesh, mm.CreateBuffer(string_view{ r_cast<const char*>(std::data(m.element_buffer)),hlp::buffer_size(m.element_buffer) }), s_cast<uint32_t>(m.element_buffer.size()));

}

}