#include "pch.h"
#include "OpenGLFBXLoader.h"
#undef min
#undef max
#include "FBXLoaderHelpers.h"

namespace idk
{
	using Vertex = fbx_loader_detail::Vertex;

	FileResources OpenGLFBXLoader::Create(FileHandle path_to_resource)
	{
		assert(Core::GetSystem<GraphicsSystem>().GetAPI() == GraphicsAPI::OpenGL);

		FileResources retval;

		Assimp::Importer importer;
		const aiScene* ai_scene = importer.ReadFile( path_to_resource.GetFullPath().data(),
													 aiProcess_Triangulate |		// Triangulates non-triangles
													 aiProcess_GenSmoothNormals |	// Generates missing normals
													 aiProcess_FlipUVs |
													 aiProcess_JoinIdenticalVertices);;

		if (ai_scene == nullptr)
			return retval;

		auto mesh_handle = Core::GetResourceManager().Create<Mesh>();
		auto& opengl_mesh = mesh_handle.as<ogl::OpenGLMesh>();
		opengl_mesh.Reset();

		vector<Vertex> vertices;
		vector<unsigned> indices;
		unsigned num_vertices = 0, num_indices = 0;

		vector<ogl::OpenGLMesh::MeshEntry> mesh_entries;

		hash_table<string, size_t> bones_table;
		vector<anim::Skeleton::Bone> bones;
		
		// Count the number of vertices and indices per mesh entry
		mesh_entries.reserve(ai_scene->mNumMeshes);
		for (size_t i = 0; i < ai_scene->mNumMeshes; ++i)
		{
			unsigned curr_base_vertex = num_vertices;
			unsigned curr_base_index = num_indices;
			unsigned curr_num_index = ai_scene->mMeshes[i]->mNumFaces * 3;

			mesh_entries.emplace_back(curr_base_vertex, curr_base_index, curr_num_index, 0);
			
			num_vertices += ai_scene->mMeshes[i]->mNumVertices;
			num_indices += curr_num_index;
		}
		opengl_mesh = ogl::OpenGLMesh{ mesh_entries };

		vertices.reserve(num_vertices);
		indices.reserve(num_indices);

		// Here, we initialize all the vertices, indices, and bones. 
		// Bones are just initialized with the offset matrix and name for now. Hierarchy will come later when we traverse the nodes.
		for (size_t i = 0; i < ai_scene->mNumMeshes; ++i)
		{
			const aiMesh* ai_mesh = ai_scene->mMeshes[i];

			// Initialize vertices
			const aiVector3D  zero{ 0.0f, 0.0f, 0.0f };
			for (size_t k = 0; k < ai_mesh->mNumVertices; ++k)
			{
				const aiVector3D& pos = ai_mesh->mVertices[k];
				const aiVector3D& normal = ai_mesh->mNormals[k];
				const aiVector3D& text = ai_mesh->HasTextureCoords(0) ? ai_mesh->mTextureCoords[0][k] : zero;

				vertices.emplace_back(Vertex{ vec3{ pos.x, pos.y, pos.z }
											 ,vec3{ normal.x, normal.y, normal.z }
											 ,vec2{ text.x, text.y } });
			}

			// Initialize indices
			for (size_t k = 0; k < ai_mesh->mNumFaces; k++)
			{
				const aiFace& face = ai_mesh->mFaces[k];
				assert(face.mNumIndices == 3);
				indices.push_back(face.mIndices[0]);
				indices.push_back(face.mIndices[1]);
				indices.push_back(face.mIndices[2]);
			}

			fbx_loader_detail::Helper::initBones(ai_mesh, vertices, mesh_entries[i]._base_vertex, bones_table, bones);
		}

		// Now we load the 

		
		
		fbx_loader_detail::Helper::initOpenGLBuffers(opengl_mesh, vertices, indices);

		retval.resources.emplace_back(mesh_handle);
		return retval;
	}

	FileResources OpenGLFBXLoader::Create(FileHandle path_to_resource, span<SerializedResourceMeta> path_to_meta)
	{
		UNREFERENCED_PARAMETER(path_to_resource);
		UNREFERENCED_PARAMETER(path_to_meta);

		return FileResources();
	}
	
}