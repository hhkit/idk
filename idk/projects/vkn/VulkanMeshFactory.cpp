#include "pch.h"
#include "VulkanMeshFactory.h"
#include <vkn/VulkanMesh.h>
#include <vkn/VulkanTypeDescriptors.h>
#include <vkn/VulkanState.h>
#include <vkn/VulkanView.h>
#include <vkn/VulkanWin32GraphicsSystem.h>

#include <core/Core.h>
#include <vkn/MemoryAllocator.h>
#include <gfx/projector_functions.h>

namespace idk::vkn
{
	using offset_t = size_t;
	template<typename CItr>
	void CpyWithStride(CItr src_start, CItr src_end, void* dst_start, void* dst_end, uint32_t dst_stride)
	{
		using T = decltype(*src_start);
		uint8_t* d_st = r_cast<uint8_t*>(dst_start);
		uint8_t* d_ed = r_cast<uint8_t*>(dst_end);
		while (src_start != src_end && dst_end > dst_start)
		{
			memcpy_s(d_st, d_ed - d_st, src_start, sizeof(T));
			d_st += dst_stride;
			++src_start;
		}
	}
	void compute_tangents(vector<vec3>& tangents, const vector<vec3>& pos, const vector<vec2>& uv, const vector<uint16_t>& indices);

	MeshFactory::MeshFactory()
	{
	}
	namespace
	{

		struct Vertex
		{
			vec3 pos;
			vec3 normal;
			vec2 uv;
		};

		auto GenerateSphereBuffer()
		{ /* create sphere mesh*/
			std::pair<vector <Vertex>, vector<uint16_t>> result;
			auto& [icosahedron, icosahedronIndices] = result;
			icosahedron.reserve(12);
			icosahedronIndices.reserve(12 * 3);

			// Assume radius to be 1.0f. This means the icohedron will be within 1 to -1.
			// We compute 5 vertices on the top and bottom row each.
			// Each vertex is spaced 72 degrees apart on the xz plane (z is up).
			// Top plane starts rotating in the xz plane at 90 - 72 = 18 degrees.
			// Bottom plane starts rotating in the xz plane at 270 + 72 = 342 degrees;
			auto topAngle = deg(18.0f);
			auto bottomAngle = deg(342.0f);
			const auto elevation = atan(0.5f);	// This was found online.
			const auto angleOffset = deg(72.0f);

			// Each vertex at +- elevation can be found by:
			// x = r*cos(elevation)*cos(72*i) 
			// y = r*cos(elevation)*sin(72*i) 
			// z = r*sin(elevation)
			const auto cosEle = cos(elevation);
			const auto sinEle = sin(elevation);

			// Add top vertex first
			icosahedron.push_back(Vertex{ vec3{ 0, 0, 1 }, vec3{ 0, 0, 1 } });

			for (unsigned i = 0; i < 5; ++i)
			{
				// Calculating the top part of the icosahedron
				const float xTop = cosEle * cos(topAngle);
				const float yTop = cosEle * sin(topAngle);

				// Calculating the bottom part of the icosahedron
				const float xBot = cosEle * cos(bottomAngle);
				const float yBot = cosEle * sin(bottomAngle);

				// For the z coordinate of top/bottom, they are simply negations of each other.
				icosahedron.push_back(Vertex{ vec3{ xTop, yTop,  sinEle }, vec3{ xTop, yTop,  sinEle } });
				icosahedron.push_back(Vertex{ vec3{ xBot, yBot, -sinEle }, vec3{ xBot, yBot, -sinEle } });

				// Offsetting the angle for the next loop
				topAngle += angleOffset;
				bottomAngle += angleOffset;
			}

			// Add bottom vertex
			icosahedron.push_back(Vertex{ vec3{ 0, 0, -1 }, vec3 {0,0,-1} });

			// Now calculate the indices
			// Icohedrop is made up of 3 rows, top, middle and bottom.
			// Calculate indices for top row
			for (uint16_t i = 1; i < 9; i += 2)
			{
				icosahedronIndices.push_back(0);
				icosahedronIndices.push_back(i);
				// Offset by 2 because there is a bottom vertex inbetween each top vertex
				icosahedronIndices.push_back(i + 2);
			}

			// Add the last triangle
			icosahedronIndices.push_back(0);
			icosahedronIndices.push_back(9);
			icosahedronIndices.push_back(1);

			// Calculate the indices in the middle
			// my my joseph this is quite cancerous
			icosahedronIndices.push_back(1); // top
			icosahedronIndices.push_back(2); // bot
			icosahedronIndices.push_back(4); // bot

			icosahedronIndices.push_back(1); // top
			icosahedronIndices.push_back(4); // bot
			icosahedronIndices.push_back(3); // top

			icosahedronIndices.push_back(3);
			icosahedronIndices.push_back(4);
			icosahedronIndices.push_back(6);

			icosahedronIndices.push_back(3);
			icosahedronIndices.push_back(6);
			icosahedronIndices.push_back(5);

			icosahedronIndices.push_back(5);
			icosahedronIndices.push_back(6);
			icosahedronIndices.push_back(8);

			icosahedronIndices.push_back(5);
			icosahedronIndices.push_back(8);
			icosahedronIndices.push_back(7);

			icosahedronIndices.push_back(7);
			icosahedronIndices.push_back(8);
			icosahedronIndices.push_back(10);

			icosahedronIndices.push_back(7);
			icosahedronIndices.push_back(10);
			icosahedronIndices.push_back(9);

			icosahedronIndices.push_back(9);
			icosahedronIndices.push_back(10);
			icosahedronIndices.push_back(2);

			icosahedronIndices.push_back(9);
			icosahedronIndices.push_back(2);
			icosahedronIndices.push_back(1);

			// Calculate the indices in the middle
			for (uint16_t i = 2; i < 10; i += 2)
			{
				icosahedronIndices.push_back(i);
				icosahedronIndices.push_back(11);
				icosahedronIndices.push_back(i + 2);
			}
			// Last index
			icosahedronIndices.push_back(10);
			icosahedronIndices.push_back(11);
			icosahedronIndices.push_back(2);

			struct compareVec
			{
				auto operator()(const vec3& lhs) const
				{
					size_t hash = std::hash<float>{}(lhs[0]);
					idk::hash_combine(hash, lhs[1]);
					idk::hash_combine(hash, lhs[2]);
					return hash;
				}
			};

			const auto addSubVert = [](std::vector<Vertex>& vertices, hash_table<vec3, uint16_t, compareVec>& shared, const vec3& pos) -> uint16_t
			{
				// Get the index IF this vertex is unique and we pushed it into the vertices vector
				auto index = static_cast<uint16_t>(vertices.size());

				// Try to insert into the set.
				const auto result = shared.emplace(pos, index);
				// Check if the insertion failed or succeeded
				if (result.second == false)
					// If we failed, we simply point index to the element that was foudn to be the same inside the set
					index = result.first->second;
				else
					// insertion into the set was successful. Meaning the vertex is unique and hence we push it into the vector
					vertices.emplace_back(Vertex{ pos, pos });

				// Return the index
				return index;
			};

			const auto subdivideIcosahedron = [addSubVert](vector<Vertex>& vertices, vector<uint16_t>& indices)
			{
				constexpr auto SUBDIVISIONS = 1;
				// Each subdivision, we subdivide every face in the CURRENT object
				for (unsigned i = 0; i < SUBDIVISIONS; ++i)
				{
					// Since the indices are not going to be correct if we were to subdivide each face, we need to clear it.
					// Save the current indices for looping.
					std::vector<uint16_t> tmpIndices{ indices };
					indices.clear();

					// Every new vertex that is created should be added to this. 
					// As 2 faces can share the same edge, we need to make sure every new half-vertex created is indeed unique. 
					hash_table<vec3, uint16_t, compareVec> sharedIndices;

					// Subdivide every face once
					for (unsigned j = 0; j < tmpIndices.size();)
					{
						const uint16_t v1Index = tmpIndices[j++];
						const uint16_t v2Index = tmpIndices[j++];
						const uint16_t v3Index = tmpIndices[j++];

						const vec3 v1 = vertices[v1Index].pos;
						const vec3 v2 = vertices[v2Index].pos;
						const vec3 v3 = vertices[v3Index].pos;

						// Compute half vertex of every edge of the triangle
						// We normalize the vertex because our radius is 1.
						// addSubVert will check if the vertex is unique or not using the sharedIndices set.
						// v1 -> v2
						const vec3 hv1 = (v1 + v2).normalize();
						const uint16_t hv1Index = addSubVert(vertices, sharedIndices, hv1);

						// v2 -> v3
						const vec3 hv2 = (v2 + v3).normalize();
						const uint16_t hv2Index = addSubVert(vertices, sharedIndices, hv2);

						// v3 -> v1
						const vec3 hv3 = (v3 + v1).normalize();
						const uint16_t hv3Index = addSubVert(vertices, sharedIndices, hv3);

						// New Indices. Every triangle will subdivide into 4 new triangles.
						indices.push_back(v1Index);
						indices.push_back(hv1Index);
						indices.push_back(hv3Index);

						indices.push_back(hv1Index);
						indices.push_back(v2Index);
						indices.push_back(hv2Index);

						indices.push_back(hv1Index);
						indices.push_back(hv2Index);
						indices.push_back(hv3Index);

						indices.push_back(hv3Index);
						indices.push_back(hv2Index);
						indices.push_back(v3Index);
					}
				}
			};

			// Subdivide the icosahedron into a sphere here.
			subdivideIcosahedron(icosahedron, icosahedronIndices);

			// project uvs
			for (auto& elem : icosahedron)
			{
				elem.uv = spherical_projection(elem.pos);
				elem.pos /= 2; // normalize
			}
			return result;
		}
	}
	void ConvertInterleaved(const vector<Vertex>& interleaved_buffer,
		const vector<uint16_t>& indices,
		vector<vec3>& pos_buffer,
		vector<vec3>& normal_buffer,
		vector<vec3>& tangent_buffer,
		vector<vec2>& uv_buffer)
	{
		pos_buffer.resize(interleaved_buffer.size());
		normal_buffer.resize(interleaved_buffer.size());
		tangent_buffer.resize(interleaved_buffer.size());
		uv_buffer.resize(interleaved_buffer.size());
		std::transform(interleaved_buffer.begin(), interleaved_buffer.end(), pos_buffer.begin(), [](const Vertex& vertex) {return vertex.pos; });
		std::transform(interleaved_buffer.begin(), interleaved_buffer.end(), normal_buffer.begin(), [](const Vertex& vertex) {return vertex.normal; });
		std::transform(interleaved_buffer.begin(), interleaved_buffer.end(), uv_buffer.begin(), [](const Vertex& vertex) {return vertex.uv; });

		compute_tangents(tangent_buffer, pos_buffer, uv_buffer, indices);
	}
	void SetMesh(
		VulkanMesh& mesh,
		MeshModder& mesh_modder,
		const vector<uint16_t>& indices,
		const vector<vec3>& pos_buffer,
		const vector<vec3>& normal_buffer,
		const vector<vec3>& tangent_buffer,
		const vector<vec2>& uv_buffer)
	{

		hash_table<attrib_index, std::pair<std::shared_ptr<MeshBuffer::Managed>, offset_t>> attribs;
		{
			auto& buffer = pos_buffer;
			//Use CreateData to create the buffer, then store the result with the offset.
			//Since it's a shared ptr, you may share the result of CreateData with multiple attrib buffers
			attribs[attrib_index::Position] = std::make_pair(mesh_modder.CreateBuffer(string_view{ r_cast<const char*>(std::data(buffer)),hlp::buffer_size(buffer) }), offset_t{ 0 });
		}
		{
			auto& buffer = tangent_buffer;
			//Use CreateData to create the buffer, then store the result with the offset.
			//Since it's a shared ptr, you may share the result of CreateData with multiple attrib buffers
			attribs[attrib_index::Tangent] = std::make_pair(mesh_modder.CreateBuffer(string_view{ r_cast<const char*>(std::data(buffer)),hlp::buffer_size(buffer) }), offset_t{ 0 });
		}

		{
			auto& buffer = normal_buffer;
			attribs[attrib_index::Normal] = std::make_pair(mesh_modder.CreateBuffer(string_view{ r_cast<const char*>(std::data(buffer)),hlp::buffer_size(buffer) }), offset_t{ 0 });
		}

		{
			auto& buffer = uv_buffer;
			attribs[attrib_index::UV] = std::make_pair(mesh_modder.CreateBuffer(string_view{ r_cast<const char*>(std::data(buffer)),hlp::buffer_size(buffer) }), offset_t{ 0 });
		}

		auto& buffer = indices;
		auto index_buffer = mesh_modder.CreateBuffer(string_view{ r_cast<const char*>(std::data(buffer)),hlp::buffer_size(buffer) });

		mesh_modder.RegisterAttribs(mesh, attribs);
		mesh_modder.SetIndexBuffer16(mesh, index_buffer, s_cast<uint32_t>(indices.size()));
	}

	void CreateSphere(VulkanMesh& sphere_mesh,MeshModder& mesh_modder)
	{

		auto [interleaved_buffer, indices] = GenerateSphereBuffer();

		vector<vec3> pos_buffer(interleaved_buffer.size());
		vector<vec3> normal_buffer(interleaved_buffer.size());
		vector<vec3> tangent_buffer(interleaved_buffer.size());
		vector<vec2> uv_buffer(interleaved_buffer.size());

		ConvertInterleaved(interleaved_buffer, indices,pos_buffer, normal_buffer, tangent_buffer, uv_buffer);


		SetMesh(sphere_mesh,mesh_modder, indices, pos_buffer, normal_buffer, tangent_buffer, uv_buffer);
		//After the map is created, pass in the remaining data as such
	}

	auto GenerateCircleInterleavedBuffers()
	{
		std::pair<vector<Vertex>, vector<uint16_t>> result;
		auto& [vertices, indices] = result;
		{	
			constexpr auto sz = 1.f;
			constexpr auto numberOfTri = 16;
			const real angle = (2.f * pi) / numberOfTri;


			for (uint16_t i = 0; i < numberOfTri; ++i)
			{
				vertices.emplace_back(Vertex{ vec3{  sz * sinf(angle * i),  0,  sz * cosf(angle * i)}, vec3{  sz * sinf(angle * i),  0,  sz * cosf(angle * i)} });

				if (i < (numberOfTri - 1))
				{
					indices.emplace_back(i);
					indices.emplace_back(static_cast<uint16_t>(i + 1));
				}
				else
				{
					indices.emplace_back(i);
					indices.emplace_back(0);
				}
			}

			for (auto& elem : vertices)
				elem.pos /= 2;

		}

		return result;
	}
	auto GeneratePlaneInterleavedBuffers()
	{

			/* create plane mesh *//* create circle mesh */
			constexpr auto sz = .5f;
			//constexpr auto numberOfTri = 16;
			//real angle = (2.f * pi) / numberOfTri;

			std::vector<Vertex> vertices{
				Vertex{ vec3{  sz,  +0.5,  sz}, vec3{0, 1,0}, vec2{0, 0} },  // front
				Vertex{ vec3{  sz,  +0.5,  -sz}, vec3{0, 1,0}, vec2{0, 1} },  // front
				Vertex{ vec3{-sz,   +0.5,  -sz},  vec3{0, 1,0}, vec2{1, 1} },  // front
				Vertex{ vec3{-sz,   +0.5,  sz},  vec3{0, 1,0}, vec2{1, 0} },  // front
			};

			std::vector<uint16_t> indices{
				0, 1, 3,
				1, 2, 3
			};
			return std::make_pair(std::move(vertices), std::move(indices));
	}
	auto GenerateFSQInterleavedBuffers()
	{
			/* create FSQ mesh */
			constexpr auto sz = 1.f;
			//constexpr auto numberOfTri = 16;
			//real angle = (2.f * pi) / numberOfTri;

			std::vector<Vertex> vertices{
				Vertex{ vec3{  sz, sz, 0}, vec3{0, 1,0}, vec2{1, 1} },  // front
				Vertex{ vec3{  sz,-sz, 0}, vec3{0, 1,0}, vec2{1, 0} },  // front
				Vertex{ vec3{-sz, -sz, 0},  vec3{0, 1,0}, vec2{0, 0} },  // front
				Vertex{ vec3{-sz,  sz, 0},  vec3{0, 1,0}, vec2{0, 1} },  // front
			};

			std::vector<uint16_t> indices{
				0, 3, 1,
				1, 3, 2
			};

		return std::make_pair(std::move(vertices),std::move(indices));
	}
	auto GenerateTetrahedronInterleavedBuffers()
	{
			std::vector<Vertex> vertices
			{
				Vertex{vec3{ 0,  0,  1}, vec3{ 0,  0,  1}},
				Vertex{vec3{ 1,  1, -1}, vec3{ 0,  1, -1}},
				Vertex{vec3{-1,  1, -1}, vec3{ 0,  1, -1}},
				Vertex{vec3{-1, -1, -1}, vec3{-1, -1, -1}},
				Vertex{vec3{ 1, -1, -1}, vec3{ 1, -1, -1}},
			};
			std::vector<uint16_t> indices
			{
				0,1,2,
				0,2,3,
				0,3,4,
				0,4,1
			};
		return std::make_pair(std::move(vertices),std::move(indices));

	}
	auto GenerateLineInterleavedBuffers()
	{
			std::vector<Vertex> vertices
			{
				Vertex{vec3{ 0, 0, 1}, vec3{ 0, 0, 1}},
				Vertex{vec3{ 0, 0,-1}, vec3{ 0, 0,-1}},
			};
			std::vector<uint16_t> indices
			{
				0,1,
			};
		return std::make_pair(std::move(vertices),std::move(indices));

	}

	void MeshFactory::GenerateDefaultMeshes()
	{
		const auto sphere_mesh = Core::GetResourceManager().LoaderEmplaceResource<VulkanMesh>(Mesh::defaults[MeshType::Sphere].guid);

		CreateSphere(*sphere_mesh,mesh_modder);
		sphere_mesh->Name("Default Sphere");


		constexpr auto sz = .5f;
		{	/* create cube mesh */
			const auto box_mesh = Mesh::defaults[MeshType::Box];
			const auto mesh_handle = Core::GetResourceManager().LoaderEmplaceResource<VulkanMesh>(box_mesh.guid);
			std::vector<Vertex> vertices{
				Vertex{ vec3{  sz,  sz,  sz}, vec3{0,0, 1} },  // front
				Vertex{ vec3{  sz, -sz,  sz}, vec3{0,0, 1} },  // front
				Vertex{ vec3{-sz, -sz,  sz}, vec3{0,0, 1} },  // front
				Vertex{ vec3{-sz,  sz,  sz}, vec3{0,0, 1} },  // front
				Vertex{ vec3{  sz,  sz, -sz}, vec3{0,0,-1} },  // back
				Vertex{ vec3{  sz, -sz, -sz}, vec3{0,0,-1} },  // back
				Vertex{ vec3{-sz, -sz, -sz}, vec3{0,0,-1} },  // back
				Vertex{ vec3{-sz,  sz, -sz}, vec3{0,0,-1} },  // back
				Vertex{ vec3{-sz,  sz,  sz}, vec3{-1,0,0} },  // left
				Vertex{ vec3{-sz,  sz, -sz}, vec3{-1,0,0} },  // left
				Vertex{ vec3{-sz, -sz, -sz}, vec3{-1,0,0} },  // left
				Vertex{ vec3{-sz, -sz,  sz}, vec3{-1,0,0} },  // left
				Vertex{ vec3{  sz,  sz,  sz}, vec3{ 1,0,0} },  // right
				Vertex{ vec3{  sz,  sz, -sz}, vec3{ 1,0,0} },  // right
				Vertex{ vec3{  sz, -sz, -sz}, vec3{ 1,0,0} },  // right
				Vertex{ vec3{  sz, -sz,  sz}, vec3{ 1,0,0} },  // right
				Vertex{ vec3{  sz,  sz,  sz}, vec3{0, 1,0} },  // top
				Vertex{ vec3{  sz,  sz, -sz}, vec3{0, 1,0} },  // top
				Vertex{ vec3{ -sz,  sz, -sz}, vec3{0, 1,0} },  // top
				Vertex{ vec3{ -sz,  sz,  sz}, vec3{0, 1,0} },  // top
				Vertex{ vec3{  sz, -sz,  sz}, vec3{0,-1,0} },  // bottom
				Vertex{ vec3{  sz, -sz, -sz}, vec3{0,-1,0} },  // bottom
				Vertex{ vec3{ -sz, -sz, -sz}, vec3{0,-1,0} },  // bottom
				Vertex{ vec3{ -sz, -sz,  sz}, vec3{0,-1,0} },  // bottom
			};

			std::vector<uint16_t> indices{
				1, 0, 3,
				2, 1, 3,

				4, 5, 7,
				5, 6, 7,

				8, 9, 11,
				9, 10, 11,

				13, 12, 15,
				14, 13, 15,

				16, 17, 19,
				17, 18, 19,

				21, 20, 23,
				22, 21, 23,
			};
			auto& interleaved_buffer = vertices;
			vector<vec3> pos_buffer(interleaved_buffer.size());
			vector<vec3> normal_buffer(interleaved_buffer.size());
			vector<vec3> tangent_buffer(interleaved_buffer.size());
			vector<vec2> uv_buffer(interleaved_buffer.size());

			ConvertInterleaved(interleaved_buffer,indices, pos_buffer, normal_buffer, tangent_buffer, uv_buffer);
			SetMesh(*mesh_handle,mesh_modder, indices, pos_buffer, normal_buffer, tangent_buffer, uv_buffer);
			mesh_handle->Name("Default Cube");
		}
		{
			vector<vec3> positions
			{
				vec3{-sz, +sz, +sz},
				vec3{-sz, +sz, -sz},
				vec3{+sz, +sz, -sz},
				vec3{+sz, +sz, +sz},
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
			hash_table<attrib_index, std::pair<std::shared_ptr<MeshBuffer::Managed>, offset_t>> attribs;
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

			auto plane_handle = Core::GetResourceManager().LoaderEmplaceResource<VulkanMesh>(Mesh::defaults[MeshType::Plane].guid);

			mesh_modder.RegisterAttribs(*plane_handle, attribs);
			mesh_modder.SetIndexBuffer16(*plane_handle, index_buffer, s_cast<uint32_t>(indices.size()));

		}

		vector<vec3> pos_buffer;//(interleaved_buffer.size;//());
		vector<vec3> normal_buffer;//(interleaved_buffer.size;//());
		vector<vec3> tangent_buffer;//(interleaved_buffer.size;//());
		vector<vec2> uv_buffer;//(interleaved_buffer.size;//());



		const auto circle_mesh = Mesh::defaults[MeshType::Circle];
		auto mesh_handle = Core::GetResourceManager().LoaderEmplaceResource<VulkanMesh>(circle_mesh.guid);
		auto pair = GenerateCircleInterleavedBuffers();
		{
			auto& [interleaved_buffer, indices] = pair;
			ConvertInterleaved(interleaved_buffer, indices, pos_buffer, normal_buffer, tangent_buffer, uv_buffer);
			SetMesh(*mesh_handle, mesh_modder, indices, pos_buffer, normal_buffer, tangent_buffer, uv_buffer);
		}

		const auto plane_mesh = Mesh::defaults[MeshType::Plane];
		mesh_handle = Core::GetResourceManager().LoaderEmplaceResource<VulkanMesh>(plane_mesh.guid);
		pair = GeneratePlaneInterleavedBuffers();
		{
			auto& [interleaved_buffer, indices] = pair;
			ConvertInterleaved(interleaved_buffer, indices, pos_buffer, normal_buffer, tangent_buffer, uv_buffer);
			SetMesh(*mesh_handle, mesh_modder, indices, pos_buffer, normal_buffer, tangent_buffer, uv_buffer);
		}

		const auto fsq_mesh = Mesh::defaults[MeshType::FSQ];
		mesh_handle = Core::GetResourceManager().LoaderEmplaceResource<VulkanMesh>(fsq_mesh.guid);
		pair = GenerateFSQInterleavedBuffers();
		{
			auto& [interleaved_buffer, indices] = pair;
			ConvertInterleaved(interleaved_buffer, indices, pos_buffer, normal_buffer, tangent_buffer, uv_buffer);
			SetMesh(*mesh_handle, mesh_modder, indices, pos_buffer, normal_buffer, tangent_buffer, uv_buffer);
		}

		const auto tet_mesh = Mesh::defaults[MeshType::Tetrahedron];
		mesh_handle = Core::GetResourceManager().LoaderEmplaceResource<VulkanMesh>(tet_mesh.guid);
		pair = GenerateTetrahedronInterleavedBuffers();
		{
			auto& [interleaved_buffer, indices] = pair;
			ConvertInterleaved(interleaved_buffer, indices, pos_buffer, normal_buffer, tangent_buffer, uv_buffer);
			SetMesh(*mesh_handle, mesh_modder, indices, pos_buffer, normal_buffer, tangent_buffer, uv_buffer);
		}

		const auto line_mesh = Mesh::defaults[MeshType::Line];
		mesh_handle = Core::GetResourceManager().LoaderEmplaceResource<VulkanMesh>(line_mesh.guid);
		pair = GenerateLineInterleavedBuffers();
		{
			auto& [interleaved_buffer, indices] = pair;
			ConvertInterleaved(interleaved_buffer, indices, pos_buffer, normal_buffer, tangent_buffer, uv_buffer);
			SetMesh(*mesh_handle, mesh_modder, indices, pos_buffer, normal_buffer, tangent_buffer, uv_buffer);
		}
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

		for (int i = 0; i+3 < indices.size(); i+=3)
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

	unique_ptr<Mesh> MeshFactory::Create() { return std::make_unique<VulkanMesh>(); }

}