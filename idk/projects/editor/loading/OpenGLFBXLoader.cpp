#include "pch.h"
#include "OpenGLFBXLoader.h"
#undef min
#undef max
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <gfx/GraphicsSystem.h>
#include <idk_opengl/resource/OpenGLMesh.h>
#include <idk_opengl/system/OpenGLGraphicsSystem.h>


namespace idk
{
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

		for (size_t i = 0; i < ai_scene->mNumMeshes; ++i)
		{
			unsigned curr_base_vertex = num_vertices;
			unsigned curr_base_index = num_indices;
			unsigned curr_num_index = ai_scene->mMeshes[i]->mNumFaces * 3;

			opengl_mesh.AddMeshEntry(curr_base_vertex, curr_base_index, curr_num_index, 0);
			num_vertices += ai_scene->mMeshes[i]->mNumVertices;
			num_indices += curr_num_index;
		}

		vertices.reserve(num_vertices);
		indices.reserve(num_indices);

		for (size_t i = 0; i < ai_scene->mNumMeshes; ++i)
		{
			const aiMesh* ai_mesh = ai_scene->mMeshes[i];

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
		}

		vector<ogl::OpenGLDescriptor> descriptor
		{
			ogl::OpenGLDescriptor{vtx::Attrib::Position, sizeof(Vertex), offsetof(Vertex, pos) },
			ogl::OpenGLDescriptor{vtx::Attrib::Normal,   sizeof(Vertex), offsetof(Vertex, normal) },
			ogl::OpenGLDescriptor{vtx::Attrib::UV,		 sizeof(Vertex), offsetof(Vertex, uv) }
		};

		opengl_mesh.AddBuffer(
			ogl::OpenGLBuffer{ GL_ARRAY_BUFFER, descriptor }
			.Bind()
			.Buffer(vertices.data(), sizeof(Vertex), s_cast<GLsizei>(vertices.size()))
		);

		opengl_mesh.AddBuffer(
			ogl::OpenGLBuffer{ GL_ELEMENT_ARRAY_BUFFER, {} }
			.Bind()
			.Buffer(indices.data(), sizeof(int), s_cast<GLsizei>(indices.size()))
		);

		retval.resources.emplace_back(mesh_handle);
		return retval;
	}

	FileResources OpenGLFBXLoader::Create(FileHandle path_to_resource, span<SerializedResourceMeta> path_to_meta)
	{
		return FileResources();
	}
}