#include "pch.h"
#include "OpenGLFBXLoader.h"
#undef min
#undef max
#include "FBXLoaderHelpers.h"

namespace idk
{
	using Vertex = fbx_loader_detail::Vertex;

	FileResources OpenGLFBXLoader::Create(PathHandle path_to_resource)
	{
		assert(Core::GetSystem<GraphicsSystem>().GetAPI() == GraphicsAPI::OpenGL);

		FileResources retval;

		Assimp::Importer importer;
		const aiScene* ai_scene = importer.ReadFile(//aiImportFileExWithProperties(// importer.ReadFile(
													path_to_resource.GetFullPath().data(),
													aiProcess_Triangulate |		// Triangulates non-triangles
													aiProcess_GenSmoothNormals |	// Generates missing normals
													aiProcess_FlipUVs |
													aiProcess_JoinIdenticalVertices);
		
		if (ai_scene == nullptr)
			return retval;
		auto mesh_handle = Core::GetResourceManager().Create<ogl::OpenGLMesh>();

		vec3 min_pos{ INT_MAX,INT_MAX ,INT_MAX }, max_pos{ INT_MIN,INT_MIN ,INT_MIN };
		vector<Vertex> vertices;
		vector<unsigned> indices;
		unsigned num_vertices = 0, num_indices = 0;

		fbx_loader_detail::BoneSet bone_set;
		fbx_loader_detail::MeshSet mesh_set;
		hash_table<string, size_t> bones_table;
		vector<anim::Skeleton::Bone> bones;
		
		// Count the number of vertices and indices per mesh entry
		for (size_t i = 0; i < ai_scene->mNumMeshes; ++i)
		{
			unsigned curr_base_vertex = num_vertices;
			unsigned curr_base_index = num_indices;
			unsigned curr_num_index = ai_scene->mMeshes[i]->mNumFaces * 3;

			mesh_handle->AddMeshEntry(curr_base_vertex, curr_base_index, curr_num_index, 0);
			
			num_vertices += ai_scene->mMeshes[i]->mNumVertices;
			num_indices += curr_num_index;
		}
		//ogl::OpenGLMesh{ mesh_entries };

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
											 ,vec2{ text.x, text.y } 
											});

				updateBounds(vertices.back().pos, min_pos, max_pos);
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

			// Push all bones that affect meshes
			for (size_t k = 0; k < ai_mesh->mNumBones; k++)
			{
				auto curr_bone = ai_mesh->mBones[k];
				bone_set.emplace(curr_bone->mName.data, fbx_loader_detail::initMat4(curr_bone->mOffsetMatrix));
			}

			// Add this mesh into the set
			mesh_set.emplace(ai_scene->mMeshes[i]->mName.data);
		}

		// Generate a graph of all the aiNodes.
		// The graph tags each node with a type and makes things a lot easier when parsing data.
		fbx_loader_detail::AssimpNode root_node;
		fbx_loader_detail::generateNodeGraph(ai_scene->mRootNode, root_node, bone_set, mesh_set);
		
		// Computing the matrix needed to normalize the mesh
		vec3 translate_vec = (max_pos + min_pos) * 0.5f;
		mat4 translate_mat = translate(-translate_vec);

		vec3 extents = max_pos - min_pos;
		float scale_factor = std::max(extents.x, std::max(extents.y, extents.z));
		vec3 scale_vec;
		scale_vec.x = 2.0f / scale_factor;
		scale_vec.y = 2.0f / scale_factor;
		scale_vec.z = 2.0f / scale_factor;
		mat4 scale_mat = mat4{ scale(scale_vec) };

		mat4 normalize_mat = scale_mat * translate_mat;// *;
		mat4 inverse_normalize_mat = translate(translate_vec)* mat4 { scale(vec3{ scale_factor / 2.0f, scale_factor / 2.0f , scale_factor / 2.0f }) };

		fbx_loader_detail::normalizeMeshEntries(vertices, normalize_mat);

		// Loads the skeleton heirarchy.
		// Passing the inverse of the normalization as the bone transformations happen in the un-normalized mesh space.
		// The inverse_normalize_mat is added to the bone_offset
		fbx_loader_detail::initBoneHierarchy(root_node, bones_table, bones, inverse_normalize_mat);

		// Loads all the vertex bone weights and indices
		fbx_loader_detail::initBoneWeights(ai_scene, mesh_handle->GetMeshEntries(), bones_table, vertices);

		// Initializes the opengl buffers
		fbx_loader_detail::initOpenGLBuffers(*mesh_handle, vertices, indices);
		retval.resources.emplace_back(RscHandle<Mesh>{mesh_handle});

		// Loading Skeletons
		auto skeleton_handle = Core::GetResourceManager().Create<anim::Skeleton>();
		auto& skeleton = skeleton_handle.as<anim::Skeleton>();

		skeleton = anim::Skeleton{ bones, bones_table };

		// Setting the skeleton transform - we multiply the normalized_mesh matrix here because the bone_transform un-does it
		mat4 skeleton_transform = normalize_mat * 
			fbx_loader_detail::initMat4(ai_scene->mRootNode->mTransformation).inverse();
		skeleton.SetSkeletonTransform(skeleton_transform);
		retval.resources.emplace_back(skeleton_handle);

		// Loading Animations
		for (size_t i = 0; i < ai_scene->mNumAnimations; ++i)
		{
			auto anim_clip_handle = Core::GetResourceManager().Create<anim::Animation>();
			auto& anim_clip = anim_clip_handle.as<anim::Animation>();
		
			// There should be a better way to do this. We are traversing the whole aiNode tree once per animation.
			fbx_loader_detail::initAnimNodes(root_node, ai_scene->mAnimations[i], anim_clip);
			retval.resources.emplace_back(anim_clip_handle);
		}
		
		return retval;
	}

	FileResources OpenGLFBXLoader::Create(PathHandle path_to_resource,const MetaFile& path_to_meta)
	{
		UNREFERENCED_PARAMETER(path_to_resource);
		UNREFERENCED_PARAMETER(path_to_meta);

		return Create(path_to_resource);

		// return FileResources();
	}

	void OpenGLFBXLoader::updateBounds(const vec3& pos, vec3& minPos, vec3& maxPos) const
	{
		// Update the bounding box size.
		if (pos.x < minPos.x)
			minPos.x = pos.x;
		if (pos.x > maxPos.x)
			maxPos.x = pos.x;

		if (pos.y < minPos.y)
			minPos.y = pos.y;
		if (pos.y > maxPos.y)
			maxPos.y = pos.y;

		if (pos.z < minPos.z)
			minPos.z = pos.z;
		if (pos.z > maxPos.z)
			maxPos.z = pos.z;
	}
	
}
