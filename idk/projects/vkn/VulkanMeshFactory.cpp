#include "pch.h"
#include "VulkanMeshFactory.h"
#include <vkn/VulkanMesh.h>
#include <vkn/VulkanTypeDescriptors.h>
#include <vkn/VulkanState.h>
#include <vkn/VulkanView.h>
#include <vkn/VulkanWin32GraphicsSystem.h>

#include <core/Core.h>
#include <vkn/MemoryAllocator.h>


namespace idk::vkn
{
	using offset_t = size_t;
	template<typename CItr>
	void CpyWithStride(CItr src_start, CItr src_end, void* dst_start, void* dst_end, uint32_t dst_stride)
	{
		using T = decltype(*src_start);
		uint8_t* d_st = r_cast<uint8_t*>(dst_start);
		uint8_t* d_ed = r_cast<uint8_t*>(dst_end);
		while (src_start != src_end && dst_end >dst_start)
		{
			memcpy_s(d_st, d_ed - d_st, src_start, sizeof(T));
			d_st += dst_stride;
			++src_start;
		}
	}


	MeshFactory::MeshFactory()
	{
	}


	unique_ptr<Mesh> MeshFactory::GenerateDefaultResource()
	{
		vector<vec3> positions
		{
			vec3{   0, 0.0f, +0.5},
			vec3{-0.5, 0.0f, -0.5},
			vec3{+0.5, 0.0f, -0.5}
		};
		vector<vec3> normals
		{
			vec3{ 0, 1, 0},
			vec3{ 0, 1, 0},
			vec3{ 0, 1, 0}
		};
		vector<vec2> uv
		{
			vec2{ 0.5f, 1},
			vec2{ 0, 0},
			vec2{ 1, 0}
		};
		vector<uint16_t> indices
		{
			0, 2, 1,
			0, 1, 2,
		};


		//@Joseph example:
		//Create this map of attrib to a pair of shared_ptr to meshbuffer::managed and offset
		hash_table<attrib_index, std::pair<std::shared_ptr<MeshBuffer::Managed>,offset_t>> attribs;
		{
			auto& buffer = positions;
			//Use CreateData to create the buffer, then store the result with the offset.
			//Since it's a shared ptr, you may share the result of CreateData with multiple attrib buffers
			attribs[attrib_index::Position] = std::make_pair(mesh_modder.CreateBuffer(string_view{ r_cast<const char*>(std::data(buffer)),hlp::buffer_size(buffer) }), offset_t{ 0 });
		}

		{
			auto& buffer = normals;
			attribs[attrib_index::Normal] = std::make_pair(mesh_modder.CreateBuffer(string_view{ r_cast<const char*>(std::data(buffer)),hlp::buffer_size(buffer) }), offset_t{ 0 });
		}

		{
			auto& buffer = uv;
			attribs[attrib_index::UV] = std::make_pair(mesh_modder.CreateBuffer(string_view{ r_cast<const char*>(std::data(buffer)),hlp::buffer_size(buffer) }), offset_t{ 0 });
		}
		
		auto& buffer = indices;
		auto index_buffer = mesh_modder.CreateBuffer(string_view{ r_cast<const char*>(std::data(buffer)),hlp::buffer_size(buffer) });
		
		auto mesh = std::make_unique<VulkanMesh>();
		mesh_modder.RegisterAttribs(*mesh, attribs);
		mesh_modder.SetIndexBuffer16(*mesh, index_buffer,s_cast<uint32_t>(indices.size()));
		//After the map is created, pass in the remaining data as such
		return mesh;
	}

	unique_ptr<Mesh> idk::vkn::MeshFactory::Create(FileHandle filepath)
	{
		return std::make_unique<VulkanMesh>();
	}

}