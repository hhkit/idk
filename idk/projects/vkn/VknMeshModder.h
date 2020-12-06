#pragma once
#include <idk.h>
#include <gfx/Mesh.h>
#include <res/ResourceFactory.h>
#include <vkn/MemoryAllocator.h>
#include <vkn/VulkanMesh.h>
namespace idk::vkn
{

	struct MeshModder
	{

		MeshModder();

		//Pass in a string view with the correct number of bytes for the buffer
		std::shared_ptr<MeshBuffer::Managed> CreateBuffer(string_view raw_data);


		void RegisterAttribs(VulkanMesh& mesh, const hash_table<attrib_index, std::pair<shared_ptr<MeshBuffer::Managed>, size_t>>& attribs);

		void SetIndexBuffer(VulkanMesh& mesh, const vector<uint16_t>& index_buffer);
		//For Index buffer with elements that are 16 bits
		void SetIndexBuffer16(VulkanMesh& mesh, shared_ptr<MeshBuffer::Managed> index_buffer, uint32_t num_indices);
		//For Index buffer with elements that are 32 bits
		void SetIndexBuffer32(VulkanMesh& mesh, shared_ptr<MeshBuffer::Managed> index_buffer, uint32_t num_indices);
	private:
		hlp::MemoryAllocator allocator;
		std::mutex create_alloc_mutex,transfer_mutex;
	};

}