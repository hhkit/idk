#include "pch.h"
#include "OpenGLFBXLoader.h"
#undef min
#undef max
#include "FBXLoaderHelpers.h"

//* // tmp include
#include <scene/SceneManager.h>
#include <core/GameObject.h>
#include <prefab/PrefabUtility.h>

#include <common/Transform.h>
#include <anim/SkinnedMeshRenderer.h>
#include <anim/AnimationController.h>
//*/

namespace idk
{
	using Vertex = fbx_loader_detail::Vertex;
	
	ResourceBundle OpenGLFBXLoader::LoadFile(PathHandle path_to_resource)
	{
		assert(Core::GetSystem<GraphicsSystem>().GetAPI() == GraphicsAPI::OpenGL);

		ResourceBundle retval;

		Assimp::Importer importer;
		const aiScene* ai_scene = importer.ReadFile(path_to_resource.GetFullPath().data(),
													aiProcess_Triangulate |			// Triangulates non-triangles
													aiProcess_GenSmoothNormals |	// Generates missing normals
													aiProcess_CalcTangentSpace |	// Generate tangents and bi-tangents
													aiProcess_FlipUVs |
													aiProcess_JoinIdenticalVertices);
		
		if (ai_scene == nullptr)
			return retval;
		
		auto scene = Core::GetSystem<SceneManager>().GetActiveScene();
		auto prefab_root = scene->CreateGameObject();
		prefab_root->Name(path_to_resource.GetStem());

		auto shader_template = *Core::GetResourceManager().Load<ShaderTemplate>("/assets/shader/pbr_forward.tmpt");
		auto h_mat = Core::GetResourceManager().Create<Material>();
		h_mat->BuildShader(shader_template, "", "");

		vec3 min_pos{ INT_MAX,INT_MAX ,INT_MAX }, max_pos{ INT_MIN,INT_MIN ,INT_MIN };
		vector<Vertex> vertices;
		vector<unsigned> indices;

		fbx_loader_detail::BoneSet bone_set;
		fbx_loader_detail::MeshSet mesh_set;
		hash_table<string, size_t> bones_table;
		vector<anim::Skeleton::Bone> bones;
		vector<RscHandle<ogl::OpenGLMesh>> meshes;

		// Initializing the bone_set
		for (size_t i = 0; i < ai_scene->mNumMeshes; ++i)
		{
			const aiMesh* ai_mesh = ai_scene->mMeshes[i];
			// Push all bones that affect meshes
			for (size_t k = 0; k < ai_mesh->mNumBones; k++)
			{
				const aiBone* ai_bone = ai_mesh->mBones[k];
				bone_set.emplace(ai_bone->mName.data, fbx_loader_detail::initMat4(ai_bone->mOffsetMatrix));
			}
		}

		// Generate a graph of all the aiNodes.
		// The graph tags each node with a type and makes things a lot easier when parsing data.
		fbx_loader_detail::AssimpNode root_node;
		fbx_loader_detail::generateNodeGraph(ai_scene->mRootNode, root_node, bone_set);

		// Initialize the bone hierarchy. This step is done before meshes cos meshes need the bone index.
		fbx_loader_detail::initBoneHierarchy(root_node, bones_table, bones);

		// Here, we initialize all the vertices, indices, and bones. 
		// Bones are just initialized with the offset matrix and name for now. Hierarchy will come later when we traverse the nodes.
		for (size_t i = 0; i < ai_scene->mNumMeshes; ++i)
		{
			const aiMesh* ai_mesh = ai_scene->mMeshes[i];
			auto mesh_handle = Core::GetResourceManager().LoaderEmplaceResource<ogl::OpenGLMesh>();
			mesh_handle->Name(ai_mesh->mName.data);

			vertices.reserve(s_cast<size_t>(ai_mesh->mNumVertices));
			indices.reserve(s_cast<size_t>(ai_mesh->mNumFaces) * 3);

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

			// Bone weights
			for (size_t k = 0; k < ai_mesh->mNumBones; ++k)
			{
				const aiBone* ai_bone = ai_mesh->mBones[k];
				auto res = bones_table.find(ai_bone->mName.data);
				assert(res != bones_table.end());

				int bone_index = static_cast<int>(res->second);
				for (size_t j = 0; j < ai_bone->mNumWeights; ++j)
				{
					float weight = ai_bone->mWeights[j].mWeight;

					unsigned vert_id = ai_bone->mWeights[j].mVertexId;
					vertices[vert_id].addBoneData(bone_index, weight);
				}
			}

			fbx_loader_detail::initOpenGLBuffers(*mesh_handle, vertices, indices);

			// Add this mesh into the set
			mesh_set.emplace(ai_scene->mMeshes[i]->mName.data);
			// meshes.push_back(mesh_handle);

			// Add mesh resource
			
			retval.Add(mesh_handle);
			
			auto mesh_child = scene->CreateGameObject();
			mesh_child->Name(mesh_handle->Name());
			mesh_child->Transform()->SetParent(prefab_root);
			auto mesh_renderer = mesh_child->AddComponent<SkinnedMeshRenderer>();
			mesh_renderer->mesh = RscHandle<Mesh>{ mesh_handle };
			mesh_renderer->material_instance.material = h_mat;

			vertices.clear();
			indices.clear();
		}

		// Computing the matrix needed to normalize the mesh
		// vec3 translate_vec = (max_pos + min_pos) * 0.5f;
		// mat4 translate_mat = translate(-translate_vec);
		// 
		// vec3 extents = max_pos - min_pos;
		// float scale_factor = std::max(extents.x, std::max(extents.y, extents.z));
		// vec3 scale_vec;
		// scale_vec.x = 2.0f / scale_factor;
		// scale_vec.y = 2.0f / scale_factor;
		// scale_vec.z = 2.0f / scale_factor;
		// mat4 scale_mat = mat4{ scale(scale_vec) };
		// 
		// mat4 normalize_mat = scale_mat * translate_mat;// *;
		// mat4 inverse_normalize_mat = translate(translate_vec)* mat4 { scale(vec3{ scale_factor / 2.0f, scale_factor / 2.0f , scale_factor / 2.0f }) };
		// 
		// fbx_loader_detail::normalizeMeshEntries(vertices, normalize_mat);

		// Loads the skeleton heirarchy.
		// Passing the inverse of the normalization as the bone transformations happen in the un-normalized mesh space.
		// The inverse_normalize_mat is added to the bone_offset
		

		// Loads all the vertex bone weights and indices
		// fbx_loader_detail::initBoneWeights(ai_scene, mesh_handle->GetMeshEntries(), bones_table, vertices);

		// Initializes the opengl buffers

		// Loading Skeletons
		auto skeleton_handle = Core::GetResourceManager().LoaderEmplaceResource<anim::Skeleton>();
		// Name is the root node
		skeleton_handle->Name(bones[0]._name);

		auto& skeleton = skeleton_handle.as<anim::Skeleton>();

		skeleton = anim::Skeleton{ bones, bones_table };

		// Setting the skeleton transform - we multiply the normalized_mesh matrix here because the bone_transform un-does it
		mat4 skeleton_transform = //normalize_mat * 
			fbx_loader_detail::initMat4(ai_scene->mRootNode->mTransformation).inverse();
		skeleton.SetSkeletonTransform(skeleton_transform);
		retval.Add(skeleton_handle);

		auto animator = prefab_root->AddComponent<AnimationController>();
		animator->SetSkeleton(skeleton_handle);

		// Loading Animations
		for (size_t i = 0; i < ai_scene->mNumAnimations; ++i)
		{
			auto anim_clip_handle = Core::GetResourceManager().LoaderEmplaceResource<anim::Animation>();
			anim_clip_handle->Name(ai_scene->mAnimations[i]->mName.data);
			auto& anim_clip = anim_clip_handle.as<anim::Animation>();

			// There should be a better way to do this. We are traversing the whole aiNode tree once per animation.
			fbx_loader_detail::initAnimNodes(root_node, ai_scene->mAnimations[i], anim_clip);
			retval.Add(anim_clip_handle);

			animator->AddAnimation(anim_clip_handle);
		}
		animator->Play(0);
		//PrefabUtility::Save(prefab_root, PathHandle{ string{"/assets/prefabs/"} + path_to_resource.GetStem().data() + ".idp" });
		return retval;
	}

	ResourceBundle OpenGLFBXLoader::LoadFile(PathHandle path_to_resource, const MetaBundle& meta_bundle)
	{
		assert(Core::GetSystem<GraphicsSystem>().GetAPI() == GraphicsAPI::OpenGL);

		ResourceBundle retval;

		Assimp::Importer importer;
		const aiScene* ai_scene = importer.ReadFile(path_to_resource.GetFullPath().data(),
			aiProcess_Triangulate |			// Triangulates non-triangles
			aiProcess_GenSmoothNormals |	// Generates missing normals
			aiProcess_CalcTangentSpace |	// Generate tangents and bi-tangents
			aiProcess_FlipUVs |
			aiProcess_JoinIdenticalVertices);

		if (ai_scene == nullptr)
			return retval;
		// auto mesh_handle = Core::GetResourceManager().Create<ogl::OpenGLMesh>();

		vec3 min_pos{ INT_MAX,INT_MAX ,INT_MAX }, max_pos{ INT_MIN,INT_MIN ,INT_MIN };
		vector<Vertex> vertices;
		vector<unsigned> indices;

		fbx_loader_detail::BoneSet bone_set;
		fbx_loader_detail::MeshSet mesh_set;
		hash_table<string, size_t> bones_table;
		vector<anim::Skeleton::Bone> bones;
		vector<RscHandle<ogl::OpenGLMesh>> meshes;

		// Initializing the bone_set
		for (size_t i = 0; i < ai_scene->mNumMeshes; ++i)
		{
			const aiMesh* ai_mesh = ai_scene->mMeshes[i];
			// Push all bones that affect meshes
			for (size_t k = 0; k < ai_mesh->mNumBones; k++)
			{
				const aiBone* ai_bone = ai_mesh->mBones[k];
				bone_set.emplace(ai_bone->mName.data, fbx_loader_detail::initMat4(ai_bone->mOffsetMatrix));
			}
		}

		// Generate a graph of all the aiNodes.
		// The graph tags each node with a type and makes things a lot easier when parsing data.
		fbx_loader_detail::AssimpNode root_node;
		fbx_loader_detail::generateNodeGraph(ai_scene->mRootNode, root_node, bone_set);

		// Initialize the bone hierarchy. This step is done before meshes cos meshes need the bone index.
		fbx_loader_detail::initBoneHierarchy(root_node, bones_table, bones);

		// Here, we initialize all the vertices, indices, and bones. 
		// Bones are just initialized with the offset matrix and name for now. Hierarchy will come later when we traverse the nodes.
		for (size_t i = 0; i < ai_scene->mNumMeshes; ++i)
		{
			const aiMesh* ai_mesh = ai_scene->mMeshes[i];
			RscHandle<ogl::OpenGLMesh> mesh_handle;

			auto rsc_exists = meta_bundle.FetchMeta(ai_mesh->mName.data);
			if(rsc_exists)
				mesh_handle = Core::GetResourceManager().LoaderEmplaceResource<ogl::OpenGLMesh>(rsc_exists->guid);
			else
				mesh_handle = Core::GetResourceManager().LoaderEmplaceResource<ogl::OpenGLMesh>();

			mesh_handle->Name(ai_mesh->mName.data);

			vertices.reserve(s_cast<size_t>(ai_mesh->mNumVertices));
			indices.reserve(s_cast<size_t>(ai_mesh->mNumFaces) * 3);

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

			// Bone weights
			for (size_t k = 0; k < ai_mesh->mNumBones; ++k)
			{
				const aiBone* ai_bone = ai_mesh->mBones[k];
				auto res = bones_table.find(ai_bone->mName.data);
				assert(res != bones_table.end());

				int bone_index = static_cast<int>(res->second);
				for (size_t j = 0; j < ai_bone->mNumWeights; ++j)
				{
					float weight = ai_bone->mWeights[j].mWeight;

					unsigned vert_id = ai_bone->mWeights[j].mVertexId;
					vertices[vert_id].addBoneData(bone_index, weight);
				}
			}

			fbx_loader_detail::initOpenGLBuffers(*mesh_handle, vertices, indices);

			// Add this mesh into the set
			mesh_set.emplace(ai_scene->mMeshes[i]->mName.data);
			// meshes.push_back(mesh_handle);

			// Add mesh resource

			retval.Add(mesh_handle);

			vertices.clear();
			indices.clear();
		}

		// Computing the matrix needed to normalize the mesh
		// vec3 translate_vec = (max_pos + min_pos) * 0.5f;
		// mat4 translate_mat = translate(-translate_vec);
		// 
		// vec3 extents = max_pos - min_pos;
		// float scale_factor = std::max(extents.x, std::max(extents.y, extents.z));
		// vec3 scale_vec;
		// scale_vec.x = 2.0f / scale_factor;
		// scale_vec.y = 2.0f / scale_factor;
		// scale_vec.z = 2.0f / scale_factor;
		// mat4 scale_mat = mat4{ scale(scale_vec) };
		// 
		// mat4 normalize_mat = scale_mat * translate_mat;// *;
		// mat4 inverse_normalize_mat = translate(translate_vec)* mat4 { scale(vec3{ scale_factor / 2.0f, scale_factor / 2.0f , scale_factor / 2.0f }) };
		// 
		// fbx_loader_detail::normalizeMeshEntries(vertices, normalize_mat);

		// Loads the skeleton heirarchy.
		// Passing the inverse of the normalization as the bone transformations happen in the un-normalized mesh space.
		// The inverse_normalize_mat is added to the bone_offset


		// Loads all the vertex bone weights and indices
		// fbx_loader_detail::initBoneWeights(ai_scene, mesh_handle->GetMeshEntries(), bones_table, vertices);

		// Initializes the opengl buffers

		// Loading Skeletons
		RscHandle<anim::Skeleton> skeleton_handle;
		{
			auto search_res = meta_bundle.FetchMeta(bones[0]._name);
			if (search_res)
				skeleton_handle = Core::GetResourceManager().LoaderEmplaceResource<anim::Skeleton>(search_res->guid);
			else
				skeleton_handle = Core::GetResourceManager().LoaderEmplaceResource<anim::Skeleton>();
		}

		// Name is the root node
		skeleton_handle->Name();

		auto& skeleton = skeleton_handle.as<anim::Skeleton>();

		skeleton = anim::Skeleton{ bones, bones_table };

		// Setting the skeleton transform - we multiply the normalized_mesh matrix here because the bone_transform un-does it
		mat4 skeleton_transform = //normalize_mat * 
			fbx_loader_detail::initMat4(ai_scene->mRootNode->mTransformation).inverse();
		skeleton.SetSkeletonTransform(skeleton_transform);
		retval.Add(skeleton_handle);

		// Loading Animations
		for (size_t i = 0; i < ai_scene->mNumAnimations; ++i)
		{
			RscHandle<anim::Animation> anim_clip_handle;
			{
				auto search_res = meta_bundle.FetchMeta(bones[0]._name);
				if (search_res)
					anim_clip_handle = Core::GetResourceManager().LoaderEmplaceResource<anim::Animation>(search_res->guid);
				else
					anim_clip_handle = Core::GetResourceManager().LoaderEmplaceResource<anim::Animation>();
			}
			anim_clip_handle->Name(ai_scene->mAnimations[i]->mName.data);

			auto& anim_clip = anim_clip_handle.as<anim::Animation>();

			// There should be a better way to do this. We are traversing the whole aiNode tree once per animation.
			fbx_loader_detail::initAnimNodes(root_node, ai_scene->mAnimations[i], anim_clip);
			retval.Add(anim_clip_handle);
		}

		return retval;
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
