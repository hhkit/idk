#include "pch.h"
#include "OpenGLMeshFactory.h"
#include <set>

#include <core/Core.h>
#include <idk_opengl/resource/OpenGLMesh.h>

namespace idk::ogl
{
	void OpenGLMeshFactory::GenerateDefaultMeshes()
	{
		struct Vertex
		{
			vec3 pos;
			vec3 normal;
		};
		vector<OpenGLDescriptor> descriptor
		{
			OpenGLDescriptor{vtx::Attrib::Position, sizeof(Vertex), offsetof(Vertex, pos) },
			OpenGLDescriptor{vtx::Attrib::Normal,   sizeof(Vertex), offsetof(Vertex, normal) }
		};
		{ /* create sphere mesh*/
			std::vector<Vertex> icosahedron;
			std::vector<int> icosahedronIndices;
			icosahedron.reserve(12);
			icosahedronIndices.reserve(12 * 3);

			// Assume radius to be 1.0f. This means the icohedron will be within 1 to -1.
			// We compute 5 vertices on the top and bottom row each.
			// Each vertex is spaced 72 degrees apart on the xz plane (z is up).
			// Top plane starts rotating in the xz plane at 90 - 72 = 18 degrees.
			// Bottom plane starts rotating in the xz plane at 270 + 72 = 342 degrees;
			auto topAngle          = deg(18.0f);
			auto bottomAngle       = deg(342.0f);
			const auto elevation   = atan(0.5f);	// This was found online.
			const auto angleOffset = deg(72.0f);

			// Each vertex at +- elevation can be found by:
			// x = r*cos(elevation)*cos(72*i) 
			// y = r*cos(elevation)*sin(72*i) 
			// z = r*sin(elevation)
			auto cosEle = cos(elevation);
			auto sinEle = sin(elevation);

			// Add top vertex first
			icosahedron.push_back(Vertex{ vec3{ 0, 0, 1 }, vec3{ 0, 0, 1 } });

			for (unsigned i = 0; i < 5; ++i)
			{
				// Calculating the top part of the icosahedron
				float xTop = cosEle * cos(topAngle);
				float yTop = cosEle * sin(topAngle);

				// Calculating the bottom part of the icosahedron
				float xBot = cosEle * cos(bottomAngle);
				float yBot = cosEle * sin(bottomAngle);

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
			for (unsigned i = 1; i < 9; i += 2)
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
			for (unsigned i = 2; i < 10; i += 2)
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

			auto addSubVert = [](std::vector<Vertex>& vertices, hash_table<vec3, int, compareVec>& shared, const vec3& pos) -> unsigned
			{
				// Get the index IF this vertex is unique and we pushed it into the vertices vector
				auto index = static_cast<int>(vertices.size());

				// Try to insert into the set.
				auto result = shared.emplace(pos, index);
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

			auto subdivideIcosahedron = [addSubVert](std::vector<Vertex>& vertices, std::vector<int>& indices)
			{
				constexpr auto SUBDIVISIONS = 1;
				// Each subdivision, we subdivide every face in the CURRENT object
				for (unsigned i = 0; i < SUBDIVISIONS; ++i)
				{
					// Since the indices are not going to be correct if we were to subdivide each face, we need to clear it.
					// Save the current indices for looping.
					std::vector<int> tmpIndices{ indices };
					indices.clear();

					// Every new vertex that is created should be added to this. 
					// As 2 faces can share the same edge, we need to make sure every new half-vertex created is indeed unique. 
					hash_table<vec3, int, compareVec> sharedIndices;

					// Subdivide every face once
					for (unsigned j = 0; j < tmpIndices.size();)
					{
						const unsigned v1Index = tmpIndices[j++];
						const unsigned v2Index = tmpIndices[j++];
						const unsigned v3Index = tmpIndices[j++];

						vec3 v1 = vertices[v1Index].pos;
						vec3 v2 = vertices[v2Index].pos;
						vec3 v3 = vertices[v3Index].pos;

						// Compute half vertex of every edge of the triangle
						// We normalize the vertex because our radius is 1.
						// addSubVert will check if the vertex is unique or not using the sharedIndices set.
						// v1 -> v2
						vec3 hv1 = (v1 + v2).normalize();
						const unsigned hv1Index = addSubVert(vertices, sharedIndices, hv1);

						// v2 -> v3
						vec3 hv2 = (v2 + v3).normalize();
						const unsigned hv2Index = addSubVert(vertices, sharedIndices, hv2);

						// v3 -> v1
						vec3 hv3 = (v3 + v1).normalize();
						const unsigned hv3Index = addSubVert(vertices, sharedIndices, hv3);

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

			auto sphere_mesh = Core::GetResourceManager().Emplace<OpenGLMesh>(Mesh::defaults[MeshType::Sphere].guid);

			sphere_mesh->AddMeshEntry(0, 0, icosahedronIndices.size(), 0);
			sphere_mesh->AddBuffer(OpenGLBuffer{ GL_ARRAY_BUFFER, descriptor }
				.Bind()
				.Buffer(icosahedron.data(), sizeof(Vertex), icosahedron.size())
			);
			sphere_mesh->AddBuffer(OpenGLBuffer{ GL_ELEMENT_ARRAY_BUFFER, {} }
				.Bind()
				.Buffer(icosahedronIndices.data(), sizeof(int), icosahedronIndices.size())
			);
		}

		{	/* create cube mesh */
			auto box_mesh = Mesh::defaults[MeshType::Box];
			auto mesh_handle = Core::GetResourceManager().Emplace<OpenGLMesh>(box_mesh.guid);
			constexpr auto sz = 1.f;
			std::vector<Vertex> vertices{
				Vertex{ vec3{  sz,  sz,  sz}, vec3{0,0, 1} },  // front
				Vertex{ vec3{  sz, -sz,  sz}, vec3{0,0, 1} },  // front
				Vertex{ vec3{- sz, -sz,  sz}, vec3{0,0, 1} },  // front
				Vertex{ vec3{- sz,  sz,  sz}, vec3{0,0, 1} },  // front
				Vertex{ vec3{  sz,  sz, -sz}, vec3{0,0,-1} },  // back
				Vertex{ vec3{  sz, -sz, -sz}, vec3{0,0,-1} },  // back
				Vertex{ vec3{- sz, -sz, -sz}, vec3{0,0,-1} },  // back
				Vertex{ vec3{- sz,  sz, -sz}, vec3{0,0,-1} },  // back
				Vertex{ vec3{- sz,  sz,  sz}, vec3{-1,0,0} },  // left
				Vertex{ vec3{- sz,  sz, -sz}, vec3{-1,0,0} },  // left
				Vertex{ vec3{- sz, -sz, -sz}, vec3{-1,0,0} },  // left
				Vertex{ vec3{- sz, -sz,  sz}, vec3{-1,0,0} },  // left
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
			std::vector<int> indices{
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

			mesh_handle->AddMeshEntry(0, 0, indices.size(), 0);

			mesh_handle->AddBuffer(
				OpenGLBuffer{ GL_ARRAY_BUFFER, descriptor }
					.Bind().Buffer(vertices.data(), sizeof(Vertex), (GLsizei) vertices.size())
			);

			mesh_handle->AddBuffer(
				OpenGLBuffer{ GL_ELEMENT_ARRAY_BUFFER, {} }
				.Bind().Buffer(indices.data(), sizeof(int), (GLsizei)indices.size())
			);
		}

		{	/* create tetrahedral mesh */
			auto tet_mesh = Mesh::defaults[MeshType::Tetrahedron];
			auto mesh_handle = Core::GetResourceManager().Emplace<OpenGLMesh>(tet_mesh.guid);
			std::vector<Vertex> vertices
			{
				Vertex{vec3{ 0,  0,  1}, vec3{ 0,  0,  1}},
				Vertex{vec3{ 1,  1, -1}, vec3{ 0,  1, -1}},
				Vertex{vec3{-1,  1, -1}, vec3{ 0,  1, -1}},
				Vertex{vec3{-1, -1, -1}, vec3{-1, -1, -1}},
				Vertex{vec3{ 1, -1, -1}, vec3{ 1, -1, -1}},
			};
			std::vector<int> indices
			{
				0,1,2,
				0,2,3,
				0,3,4,
				0,4,1
			};

			mesh_handle->AddMeshEntry(0, 0, indices.size(), 0);

			mesh_handle->AddBuffer(
				OpenGLBuffer{ GL_ARRAY_BUFFER, descriptor }
				.Bind().Buffer(vertices.data(), sizeof(Vertex), (GLsizei)vertices.size())
			);

			mesh_handle->AddBuffer(
				OpenGLBuffer{ GL_ELEMENT_ARRAY_BUFFER, {} }
				.Bind().Buffer(indices.data(), sizeof(int), (GLsizei)indices.size())
			);
		}

		{	/* create line mesh */
			auto line_mesh = Mesh::defaults[MeshType::Line];
			auto mesh_handle = Core::GetResourceManager().Emplace<OpenGLMesh>(line_mesh.guid);
			std::vector<Vertex> vertices
			{
				Vertex{vec3{ 0, 0, 1}, vec3{ 0, 0, 1}},
				Vertex{vec3{ 0, 0,-1}, vec3{ 0, 0,-1}},
			};
			std::vector<int> indices
			{
				0,1,
			};

			mesh_handle->SetDrawMode(GL_LINES);

			mesh_handle->AddMeshEntry(0, 0, indices.size(), 0);

			mesh_handle->AddBuffer(
				OpenGLBuffer{ GL_ARRAY_BUFFER, descriptor }
				.Bind().Buffer(vertices.data(), sizeof(Vertex), (GLsizei)vertices.size())
			);

			mesh_handle->AddBuffer(
				OpenGLBuffer{ GL_ELEMENT_ARRAY_BUFFER, {} }
				.Bind().Buffer(indices.data(), sizeof(int), (GLsizei)indices.size())
			);
		}
	}

	unique_ptr<Mesh> OpenGLMeshFactory::GenerateDefaultResource()
	{
		auto retval = std::make_unique<OpenGLMesh>();
		struct Vertex
		{
			vec3 pos;
			vec3 normal;
		};

		vector<OpenGLDescriptor> descriptor
		{
			OpenGLDescriptor{vtx::Attrib::Position, sizeof(Vertex), offsetof(Vertex, pos) },
			OpenGLDescriptor{vtx::Attrib::Normal,   sizeof(Vertex), offsetof(Vertex, normal) }
		};

		vector<Vertex> vertexes
		{
			Vertex{vec3{0,    +0.5, 0}},
			Vertex{vec3{-0.5, -0.5, 0}},
			Vertex{vec3{+0.5, -0.5, 0}}
		};
		retval->AddMeshEntry(0, 0, 3, 0);
		retval->AddBuffer( 
			OpenGLBuffer{ GL_ARRAY_BUFFER, descriptor }
				.Bind()
				.Buffer(vertexes.data(), sizeof(Vertex), s_cast<GLsizei>(vertexes.size()))
		);

		vector<int> indices
		{
			0, 2, 1
		};

		retval->AddBuffer(
			OpenGLBuffer{ GL_ELEMENT_ARRAY_BUFFER, {} }
			.Bind()
			.Buffer(indices.data(), sizeof(int), s_cast<GLsizei>(indices.size()))
		);

		return retval;
	}

	unique_ptr<Mesh> OpenGLMeshFactory::Create()
	{
		return std::make_unique<OpenGLMesh>();
	}

	unique_ptr<Mesh> OpenGLMeshFactory::Create(PathHandle)
	{
		return unique_ptr<Mesh>();
	}
}