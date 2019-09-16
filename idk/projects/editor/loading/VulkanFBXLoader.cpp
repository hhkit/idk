#include "pch.h"
#include "VulkanFBXLoader.h"
#undef min
#undef max
#include "FBXLoaderHelpers.h"
#include "vkn/BufferHelpers.h"
#include "vkn/VknMeshModder.h"

namespace idk
{
	using Vertex = fbx_loader_detail::Vertex;

	struct VulkanFBXLoader::Data
	{
		vkn::MeshModder _modder;
	};
	VulkanFBXLoader::VulkanFBXLoader() :_data{ std::make_unique<Data>() }
	{
	}

	VulkanFBXLoader::~VulkanFBXLoader() = default;
	FileResources VulkanFBXLoader::Create(FileHandle path_to_resource)
	{
		struct MeshEntry
		{
			unsigned _base_vertex = 0;
			unsigned _base_index = 0;
			unsigned _num_index = 0;
			unsigned _texture_index = 0;
		};

		assert(Core::GetSystem<GraphicsSystem>().GetAPI() == GraphicsAPI::Vulkan);

		FileResources retval;

		Assimp::Importer importer;
		const aiScene* ai_scene = importer.ReadFile(path_to_resource.GetFullPath().data(),
													aiProcess_Triangulate |		// Triangulates non-triangles
													aiProcess_GenSmoothNormals |	// Generates missing normals
													aiProcess_FlipUVs |
													aiProcess_JoinIdenticalVertices);;

		if (ai_scene == nullptr)
			return retval;

		auto mesh_handle = Core::GetResourceManager().Create<vkn::VulkanMesh>();

		vector<vec3	>	positions;
		vector<vec3	>	normals;
		vector<vec2	>	uvs;
		vector<ivec4>	bone_ids;
		vector<vec4	>	bone_weights;

		vector<unsigned> indices;
		unsigned num_vertices = 0, num_indices = 0;

		vector<MeshEntry> mesh_entries;

		fbx_loader_detail::BoneSet bones_set{ fbx_loader_detail::BoneData{ai_scene->mRootNode->mName.data} };
		hash_table<string, size_t> bones_table;
		vector<anim::Skeleton::Bone> bones;

		// Count the number of vertices and indices per mesh entry
		mesh_entries.reserve(ai_scene->mNumMeshes);
		for (size_t i = 0; i < ai_scene->mNumMeshes; ++i)
		{
			unsigned curr_base_vertex = num_vertices;
			unsigned curr_base_index = num_indices;
			unsigned curr_num_index = ai_scene->mMeshes[i]->mNumFaces * 3;

			mesh_entries.push_back(MeshEntry{ curr_base_vertex, curr_base_index, curr_num_index, 0 });

			num_vertices += ai_scene->mMeshes[i]->mNumVertices;
			num_indices += curr_num_index;
		}
		
		positions.reserve(num_vertices);
		normals.reserve(num_vertices);
		uvs.reserve(num_vertices);
		bone_ids.reserve(num_vertices);
		bone_weights.reserve(num_vertices);

		indices.reserve(num_indices);

		// Here, we initialize all the vertices, indices, and bones. 
		// Bones are just initialized with the offset matrix and name for now. Hierarchy will come later when we traverse the nodes.
		for (size_t i = 0; i < ai_scene->mNumMeshes; ++i)
		{
			const aiMesh* ai_mesh = ai_scene->mMeshes[i];
			const MeshEntry& entry = mesh_entries[i];
			// Initialize vertices
			const aiVector3D  zero{ 0.0f, 0.0f, 0.0f };
			for (size_t k = 0; k < ai_mesh->mNumVertices; ++k)
			{
				const aiVector3D& pos = ai_mesh->mVertices[k];
				const aiVector3D& normal = ai_mesh->mNormals[k];
				const aiVector3D& text = ai_mesh->HasTextureCoords(0) ? ai_mesh->mTextureCoords[0][k] : zero;
				positions.emplace_back(pos.x, pos.y, pos.z);
				normals.emplace_back(normal.x, normal.y, normal.z);
				uvs.emplace_back(text.x, text.y);
			}

			// Initialize indices
			for (size_t k = 0; k < ai_mesh->mNumFaces; k++)
			{
				const aiFace& face = ai_mesh->mFaces[k];
				assert(face.mNumIndices == 3);
				indices.push_back(face.mIndices[0] + entry._base_vertex);
				indices.push_back(face.mIndices[1] + entry._base_vertex);
				indices.push_back(face.mIndices[2] + entry._base_vertex);
			}

			// Push all bones that affect meshes
			for (size_t k = 0; k < ai_mesh->mNumBones; k++)
			{
				auto curr_bone = ai_mesh->mBones[k];
				bones_set.emplace(curr_bone->mName.data, fbx_loader_detail::initMat4(curr_bone->mOffsetMatrix));
			}
		}

		// // Loads the skeleton heirarchy
		// fbx_loader_detail::initBoneHierarchy(ai_scene->mRootNode, bones_set, bones_table, bones);
		// 
		// // Loads all the vertex bone weights and indices
		// fbx_loader_detail::initBoneWeights(ai_scene, mesh_handle->GetMeshEntries(), bones_table, vertices);
		using attrib_index = vkn::attrib_index;
		using MeshBuffer = vkn::MeshBuffer;
		using MeshModder = vkn::MeshModder;
		using offset_t = size_t;
		auto& mesh_modder = _data->_modder;
		hash_table<attrib_index, std::pair<std::shared_ptr<MeshBuffer::Managed>, offset_t>> attribs;
		{
			auto& buffer = positions;
			//Use CreateData to create the buffer, then store the result with the offset.
			//Since it's a shared ptr, you may share the result of CreateData with multiple attrib buffers
			attribs[vkn::attrib_index::Position] = std::make_pair(mesh_modder.CreateBuffer(string_view{ r_cast<const char*>(std::data(buffer)),vkn::hlp::buffer_size(buffer) }), offset_t{ 0 });
		}

		{
			auto& buffer = normals;
			attribs[attrib_index::Normal] = std::make_pair(mesh_modder.CreateBuffer(string_view{ r_cast<const char*>(std::data(buffer)),vkn::hlp::buffer_size(buffer) }), offset_t{ 0 });
		}

		{
			auto& buffer = uvs;
			attribs[attrib_index::UV] = std::make_pair(mesh_modder.CreateBuffer(string_view{ r_cast<const char*>(std::data(buffer)),vkn::hlp::buffer_size(buffer) }), offset_t{ 0 });
		}

		auto& buffer = indices;
		auto index_buffer = mesh_modder.CreateBuffer(string_view{ r_cast<const char*>(std::data(buffer)),vkn::hlp::buffer_size(buffer) });

		mesh_modder.RegisterAttribs(*mesh_handle, attribs);
		mesh_modder.SetIndexBuffer32(*mesh_handle, index_buffer, s_cast<uint32_t>(indices.size()));
		retval.resources.emplace_back(RscHandle<Mesh>{mesh_handle});

		return retval;
	}

	FileResources VulkanFBXLoader::Create(FileHandle path_to_resource, const MetaFile& path_to_meta)
	{
		UNREFERENCED_PARAMETER(path_to_resource);
		UNREFERENCED_PARAMETER(path_to_meta);

		return Create(path_to_resource);

		// return FileResources();
	}
	

}
