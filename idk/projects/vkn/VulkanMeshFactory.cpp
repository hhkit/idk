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

	vec3 compute_tangent(vec3 p0, vec3 p1, vec3 p2, vec2 uv0, vec2 uv1, vec2 uv2)
	{
		vec3 p01 = p1 - p0;
		vec3 p02 = p2 - p0;
		[[maybe_unused]] float u02 = uv2.x - uv0.x;
		[[maybe_unused]] float u01 = uv1.x - uv0.x;

		float v02 = uv2.y - uv0.y;
		float v01 = uv1.y - uv0.y;

		//float m = u01 * v02 - u02 * v01;

		return v02 * p01 - v01 * p02;
	}
	template<typename T>
	T zero_error(T val)
	{
		return (abs(val) <= constants::epsilon<T>()) ? 0 : val;
	}

	//Treats the indices as a triangle list
	void compute_tangents(vector<vec3>& tangents, const vector<vec3>& pos, const vector<vec2>& uv, const vector<uint16_t>& indices)
	{
		tangents.resize(0);//clear
		tangents.resize(pos.size());

		for (int i = 0; i < indices.size()-3; i+=3)
		{
			auto i0 = indices[i], i1 = indices[i+1], i2 = indices[i+2];
			vec3 tangent = compute_tangent(pos[i0], pos[i1], pos[i2], uv[i0], uv[i1], uv[i2]);

			vec3 e01 = pos[i1] - pos[i0];
			vec3 e02 = pos[i2] - pos[i0];
			vec3 e12 = pos[i2] - pos[i1];
			//N0 = E0 X  E1
			real n0 =  e01.dot(e02);
			//N1 = E2 X -E0
			real n1 =  (-e01).dot(e12);
			//N2 = -E1 X -E2  
			real n2 = (-e02).dot(-e12);


			tangents[i0] = tangent * s_cast<real>(acos(zero_error(n0)));
			tangents[i1] = tangent * s_cast<real>(acos(zero_error(n1)));
			tangents[i2] = tangent * s_cast<real>(acos(zero_error(n2)));

		}
		for (auto& tangent : tangents)
		{
			tangent.normalize();
		}

	}


	unique_ptr<Mesh> MeshFactory::GenerateDefaultResource()
	{
		vector<vec3> positions
		{
			vec3{-0.5, 0.0f, +0.5},
			vec3{-0.5, 0.0f, -0.5},
			vec3{+0.5, 0.0f, -0.5},
			vec3{+0.5, 0.0f, +0.5},
		};
		vector<vec3> normals
		{
			vec3{ 0, 1, 0},
			vec3{ 0, 1, 0},
			vec3{ 0, 1, 0},
			vec3{ 0, 1, 0}
		};
		vector<vec3> tangents
		{
			vec3{ 1, 0, 0},
			vec3{ 1, 0, 0},
			vec3{ 1, 0, 0},
			vec3{ 1, 0, 0}
		};
		vector<vec2> uv
		{
			vec2{ 0, 1},
			vec2{ 0, 0},
			vec2{ 1, 0},
			vec2{ 1, 1}
		};
		vector<uint16_t> indices
		{
			0, 2, 1,
			3, 2, 0,
		};
		compute_tangents(tangents, positions, uv, indices);

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
			auto& buffer = tangents;
			//Use CreateData to create the buffer, then store the result with the offset.
			//Since it's a shared ptr, you may share the result of CreateData with multiple attrib buffers
			attribs[attrib_index::Tangent] = std::make_pair(mesh_modder.CreateBuffer(string_view{ r_cast<const char*>(std::data(buffer)),hlp::buffer_size(buffer) }), offset_t{ 0 });
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

}