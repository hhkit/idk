#include "pch.h"
#include "VulkanMesh.h"
#include <gfx/CompiledMesh.h>
#include <vkn/VknMeshModder.h>
#include <vkn/BufferHelpers.h>
#include <vkn/VulkanMeshFactory.h>
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>
#include <vkn/BufferHelpers.inl>

#include <idk/parallel/ThreadPool.h>

#define HANDLE_DEFAULT(call_on_default)				\
if (use_default_)									\
{													\
auto& default_obj =*RscHandle<VulkanMesh>{};		\
	if(&default_obj!=this)							\
		return default_obj.call_on_default;         \
}													
#include <atomic>
static std::atomic<int> loading_counter = 0;
static std::atomic<int> loaded_counter = 0;
namespace idk::vkn
{
	VulkanMesh::VulkanMesh(CompiledMesh&& compiled_mesh) :use_default_{true}
	{
		auto& m = compiled_mesh;
		Core::GetThreadPool().Post([this,m = std::move(compiled_mesh)]()
		{
			++loading_counter;
			bounding_volume = m.bounding_volume;
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
			mm.RegisterAttribs(*this, attribs);
			mm.SetIndexBuffer32(*this, mm.CreateBuffer(string_view{ r_cast<const char*>(std::data(m.element_buffer)),hlp::buffer_size(m.element_buffer) }), s_cast<uint32_t>(m.element_buffer.size()));
			//We done
			use_default(false);
			++loaded_counter;
		}
		);
	}
	//// 
	const MeshBuffer& VulkanMesh::Get(attrib_index index) const
	{
		HANDLE_DEFAULT(Get(index));
		auto itr = buffers.find(index);
		if (itr != buffers.end())
			return itr->second;
		throw std::exception{ "Attempting to get invalid attrib index from vulkan mesh." };
		//return MeshBuffer{};
	}
	bool VulkanMesh::Has(attrib_index index) const
	{
		HANDLE_DEFAULT(Has(index));
		auto itr = buffers.find(index);
		return (itr != buffers.end());
	}
	const VulkanMesh::buffers_t& VulkanMesh::Buffers() const 
	{
		HANDLE_DEFAULT(Buffers());
		return buffers;
	}
	int VulkanMesh::GetAttribs() const
{
	//TODO Actually get the attribs
	return 0;
}

const std::optional<MeshBuffer>& VulkanMesh::GetIndexBuffer() const 
{
	HANDLE_DEFAULT(GetIndexBuffer());
	return index_buffer;
}

void VulkanMesh::SetIndexBuffer(MeshBuffer&& buffer, uint32_t count, vk::IndexType type) { index_buffer = std::move(buffer); index_count = count; index_type = type; }

void VulkanMesh::SetBuffer(attrib_index type, MeshBuffer&& buffer)
{
	buffers[type] = std::move(buffer);
}
#pragma optimize("",off)
void VulkanMesh::use_default(bool value)
{
	use_default_ = value;
}

}