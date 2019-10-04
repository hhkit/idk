#include "pch.h"
#include "VulkanFBXLoader.h"
#undef min
#undef max
#include "FBXLoaderHelpers.h"
#include "vkn/BufferHelpers.h"
#include "vkn/VknMeshModder.h"

//* // tmp include
#include <scene/SceneManager.h>
#include <file/FileSystem.h>
#include <core/GameObject.h>
#include <prefab/PrefabUtility.h>
#include <gfx/ShaderGraph.h>
#include <common/Transform.h>
#include <anim/SkinnedMeshRenderer.h>
#include <anim/Animator.h>
//*/

namespace idk
{
	struct VulkanFBXLoader::Data
	{
		vkn::MeshModder _modder;
	};

	ResourceBundle VulkanFBXLoader::LoadFile(PathHandle path_to_resource)
	{
		assert(Core::GetSystem<GraphicsSystem>().GetAPI() == GraphicsAPI::Vulkan);

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

		const auto scene = Core::GetSystem<SceneManager>().GetActiveScene();
		const auto prefab_root = scene->CreateGameObject();
		prefab_root->Name(path_to_resource.GetStem());
		// prefab_root->Transform()->scale /= 200.0f;

		using attrib_index = vkn::attrib_index;
		using MeshBuffer = vkn::MeshBuffer;
		using MeshModder = vkn::MeshModder;
		using offset_t = size_t;
		auto& mesh_modder = _data->_modder;

		auto shader_template = *Core::GetResourceManager().Load<ShaderTemplate>("/assets/shader/pbr_forward.tmpt");
		auto h_mat = *Core::GetResourceManager().Load<shadergraph::Graph>("/assets/materials/test.mat");
		h_mat->Compile();
		const auto mat_inst = Core::GetResourceManager().Create<MaterialInstance>();
		mat_inst->material = h_mat;

		fbx_loader_detail::BoneSet bone_set;
		fbx_loader_detail::MeshSet mesh_set;
		hash_table<string, size_t> bones_table;
		vector<anim::Bone> bones;

		vector<vec3	>	positions;
		vector<vec3	>	normals;
		vector<vec2	>	uvs;
		vector<vec3	>	tangents;
		vector<vec3	>	bi_tangents;
		vector<ivec4>	bone_ids;
		vector<vec4	>	bone_weights;

		vector<unsigned> indices;
		
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
		vector<fbx_loader_detail::AssimpNode> assimp_node_vec;
		fbx_loader_detail::generateNodeGraph(ai_scene->mRootNode, assimp_node_vec, bone_set);

		// Initialize the bone hierarchy. This step is done before meshes cos meshes need the bone index.
		fbx_loader_detail::initBoneHierarchy(assimp_node_vec, bones_table, bones);

		// Here, we initialize all the vertices, indices, and bones. 
		// Bones are just initialized with the local bind pose matrix and name for now.
		for (size_t i = 0; i < ai_scene->mNumMeshes; ++i)
		{
			const aiMesh* ai_mesh = ai_scene->mMeshes[i];
			const auto mesh_handle = Core::GetResourceManager().LoaderEmplaceResource<vkn::VulkanMesh>();
			mesh_handle->Name(ai_mesh->mName.data);

			positions.reserve(s_cast<size_t>(ai_mesh->mNumVertices));
			normals.reserve(s_cast<size_t>(ai_mesh->mNumVertices));
			uvs.reserve(s_cast<size_t>(ai_mesh->mNumVertices));
			tangents.reserve(s_cast<size_t>(ai_mesh->mNumVertices));
			bi_tangents.reserve(s_cast<size_t>(ai_mesh->mNumVertices));

			// Bone weights are resized because we need to do subscript directly
			bone_ids.resize(s_cast<size_t>(ai_mesh->mNumVertices), ivec4{ 0,0,0,0 });
			bone_weights.resize(s_cast<size_t>(ai_mesh->mNumVertices), vec4{ 0,0,0,0 });

			indices.reserve(s_cast<size_t>(ai_mesh->mNumFaces) * 3);

			// Initialize vertices
			const aiVector3D  zero{ 0.0f, 0.0f, 0.0f };
			for (size_t k = 0; k < ai_mesh->mNumVertices; ++k)
			{
				const aiVector3D& pos = ai_mesh->mVertices[k];
				const aiVector3D& normal = ai_mesh->mNormals[k];
				const aiVector3D& text = ai_mesh->HasTextureCoords(0) ? ai_mesh->mTextureCoords[0][k] : zero;
				aiVector3D tangent, bi_tangent;
				if (ai_mesh->HasTangentsAndBitangents())
				{
					tangent = ai_mesh->mTangents[k];
					bi_tangent = ai_mesh->mBitangents[k];
				}

				positions.emplace_back(vec3{ pos.x, pos.y, pos.z });
				normals.emplace_back(vec3{ normal.x, normal.y, normal.z });
				uvs.emplace_back(vec2{ text.x, text.y });
				tangents.emplace_back(vec3{ tangent.x, tangent.y, tangent.z });
				bi_tangents.emplace_back(vec3{ bi_tangent.x, bi_tangent.y, bi_tangent.z });


				// updateBounds(vertices.back().pos, min_pos, max_pos);
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

				const int bone_index = static_cast<int>(res->second);
				for (size_t j = 0; j < ai_bone->mNumWeights; ++j)
				{
					const float weight = ai_bone->mWeights[j].mWeight;
					const unsigned vert_id = ai_bone->mWeights[j].mVertexId;
					fbx_loader_detail::addBoneData(bone_index, weight, bone_ids[vert_id], bone_weights[vert_id]);
				}
			}

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
			{
				auto& buffer = tangents;
				attribs[attrib_index::Tangent] = std::make_pair(mesh_modder.CreateBuffer(string_view{ r_cast<const char*>(std::data(buffer)),vkn::hlp::buffer_size(buffer) }), offset_t{ 0 });
			}
			{
				auto& buffer = bi_tangents;
				attribs[attrib_index::Bitangent] = std::make_pair(mesh_modder.CreateBuffer(string_view{ r_cast<const char*>(std::data(buffer)),vkn::hlp::buffer_size(buffer) }), offset_t{ 0 });
			}
			{
				auto& buffer = bone_ids;
				attribs[attrib_index::BoneID] = std::make_pair(mesh_modder.CreateBuffer(string_view{ r_cast<const char*>(std::data(buffer)),vkn::hlp::buffer_size(buffer) }), offset_t{ 0 });
			}
			{
				auto& buffer = bone_weights;
				attribs[attrib_index::BoneWeight] = std::make_pair(mesh_modder.CreateBuffer(string_view{ r_cast<const char*>(std::data(buffer)),vkn::hlp::buffer_size(buffer) }), offset_t{ 0 });
			}

			auto& buffer = indices;
			auto index_buffer = mesh_modder.CreateBuffer(string_view{ r_cast<const char*>(std::data(buffer)),vkn::hlp::buffer_size(buffer) });

			mesh_modder.RegisterAttribs(*mesh_handle, attribs);
			mesh_modder.SetIndexBuffer32(*mesh_handle, index_buffer, s_cast<uint32_t>(indices.size()));

			// Create mesh objects
			const auto mesh_child = scene->CreateGameObject();
			mesh_child->Name(mesh_handle->Name());
			mesh_child->Transform()->SetParent(prefab_root);
			const auto mesh_renderer = mesh_child->AddComponent<SkinnedMeshRenderer>();
			mesh_renderer->mesh = RscHandle<Mesh>{ mesh_handle };
			mesh_renderer->material_instance = mat_inst;

			// Clear the buffer
			positions.clear();
			normals.clear();
			uvs.clear();
			tangents.clear();
			bi_tangents.clear();
			bone_ids.clear();
			bone_weights.clear();

			indices.clear();

			retval.Add(mesh_handle);
		}

		// Loading Skeletons
		const auto skeleton_handle = Core::GetResourceManager().LoaderEmplaceResource<anim::Skeleton>();
		// Name is the root node
		skeleton_handle->Name(bones[0]._name);

		auto& skeleton = skeleton_handle.as<anim::Skeleton>();

		skeleton = anim::Skeleton{ bones, bones_table };

		// Setting the skeleton transform - we multiply the normalized_mesh matrix here because the bone_transform un-does it
		const mat4 skeleton_transform = fbx_loader_detail::initMat4(ai_scene->mRootNode->mTransformation).inverse();
		skeleton.SetSkeletonTransform(skeleton_transform);
		retval.Add(skeleton_handle);

		const auto animator = prefab_root->AddComponent<Animator>();
		animator->SetSkeleton(skeleton_handle);

		// Loading Animations
		for (size_t i = 0; i < ai_scene->mNumAnimations; ++i)
		{
			const auto anim_clip_handle = Core::GetResourceManager().LoaderEmplaceResource<anim::Animation>();
			anim_clip_handle->Name(ai_scene->mAnimations[i]->mName.data);
			auto& anim_clip = anim_clip_handle.as<anim::Animation>();

			// There should be a better way to do this. We are traversing the whole aiNode tree once per animation.
			fbx_loader_detail::initAnimNodes(assimp_node_vec, ai_scene->mAnimations[i], anim_clip);
			retval.Add(anim_clip_handle);

			animator->AddAnimation(anim_clip_handle);
		}
		// animator->Play(0);
		// Saving the prefab
		const auto prefab_handle = PrefabUtility::Save(prefab_root, string{ "/assets/prefabs/" } +path_to_resource.GetStem().data() + ".idp");
		retval.Add(prefab_handle);

		scene->DestroyGameObject(prefab_root);
		return retval;

		
		// // Loads the skeleton heirarchy
		// fbx_loader_detail::initBoneHierarchy(ai_scene->mRootNode, bones_set, bones_table, bones);
		// 
		// // Loads all the vertex bone weights and indices
		// fbx_loader_detail::initBoneWeights(ai_scene, mesh_handle->GetMeshEntries(), bones_table, vertices);
		
		// hash_table<attrib_index, std::pair<std::shared_ptr<MeshBuffer::Managed>, offset_t>> attribs;
		// {
		// 	auto& buffer = positions;
		// 	//Use CreateData to create the buffer, then store the result with the offset.
		// 	//Since it's a shared ptr, you may share the result of CreateData with multiple attrib buffers
		// 	attribs[vkn::attrib_index::Position] = std::make_pair(mesh_modder.CreateBuffer(string_view{ r_cast<const char*>(std::data(buffer)),vkn::hlp::buffer_size(buffer) }), offset_t{ 0 });
		// }
		// 
		// {
		// 	auto& buffer = normals;
		// 	attribs[attrib_index::Normal] = std::make_pair(mesh_modder.CreateBuffer(string_view{ r_cast<const char*>(std::data(buffer)),vkn::hlp::buffer_size(buffer) }), offset_t{ 0 });
		// }
		// 
		// {
		// 	auto& buffer = uvs;
		// 	attribs[attrib_index::UV] = std::make_pair(mesh_modder.CreateBuffer(string_view{ r_cast<const char*>(std::data(buffer)),vkn::hlp::buffer_size(buffer) }), offset_t{ 0 });
		// }
		// 
		// auto& buffer = indices;
		// auto index_buffer = mesh_modder.CreateBuffer(string_view{ r_cast<const char*>(std::data(buffer)),vkn::hlp::buffer_size(buffer) });
		// 
		// mesh_modder.RegisterAttribs(*mesh_handle, attribs);
		// mesh_modder.SetIndexBuffer32(*mesh_handle, index_buffer, s_cast<uint32_t>(indices.size()));
		// retval.Add(mesh_handle);
		// 
		// return retval;
	}
	ResourceBundle VulkanFBXLoader::LoadFile(PathHandle path_to_resource, const MetaBundle& meta_bundle)
	{
		assert(Core::GetSystem<GraphicsSystem>().GetAPI() == GraphicsAPI::Vulkan);

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

		const auto scene = Core::GetSystem<SceneManager>().GetActiveScene();
		const auto prefab_root = scene->CreateGameObject();
		prefab_root->Name(path_to_resource.GetStem());
		// prefab_root->Transform()->scale /= 200.0f;

		using attrib_index = vkn::attrib_index;
		using MeshBuffer = vkn::MeshBuffer;
		using MeshModder = vkn::MeshModder;
		using offset_t = size_t;
		auto& mesh_modder = _data->_modder;

		auto shader_template = *Core::GetResourceManager().Load<ShaderTemplate>("/assets/shader/pbr_forward.tmpt");
		auto h_mat = *Core::GetResourceManager().Load<shadergraph::Graph>("/assets/materials/test.mat");
		h_mat->Compile();
		const auto mat_inst = Core::GetResourceManager().Create<MaterialInstance>();
		mat_inst->material = h_mat;

		fbx_loader_detail::BoneSet bone_set;
		fbx_loader_detail::MeshSet mesh_set;
		hash_table<string, size_t> bones_table;
		vector<anim::Bone> bones;

		vector<vec3	>	positions;
		vector<vec3	>	normals;
		vector<vec2	>	uvs;
		vector<vec3	>	tangents;
		vector<vec3	>	bi_tangents;
		vector<ivec4>	bone_ids;
		vector<vec4	>	bone_weights;

		vector<unsigned> indices;

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
		vector<fbx_loader_detail::AssimpNode> assimp_node_vec;
		fbx_loader_detail::generateNodeGraph(ai_scene->mRootNode, assimp_node_vec, bone_set);

		// Initialize the bone hierarchy. This step is done before meshes cos meshes need the bone index.
		fbx_loader_detail::initBoneHierarchy(assimp_node_vec, bones_table, bones);

		// Here, we initialize all the vertices, indices, and bones. 
		// Bones are just initialized with the local bind pose matrix and name for now.
		for (size_t i = 0; i < ai_scene->mNumMeshes; ++i)
		{
			const aiMesh* ai_mesh = ai_scene->mMeshes[i];
			RscHandle<vkn::VulkanMesh> mesh_handle;
			{
				auto rsc_exists = meta_bundle.FetchMeta(ai_mesh->mName.data);
				if (rsc_exists)
					mesh_handle = Core::GetResourceManager().LoaderEmplaceResource<vkn::VulkanMesh>(rsc_exists->guid);
				else
					mesh_handle = Core::GetResourceManager().LoaderEmplaceResource<vkn::VulkanMesh>();
			}
			mesh_handle->Name(ai_mesh->mName.data);

			positions.reserve(s_cast<size_t>(ai_mesh->mNumVertices));
			normals.reserve(s_cast<size_t>(ai_mesh->mNumVertices));
			uvs.reserve(s_cast<size_t>(ai_mesh->mNumVertices));
			tangents.reserve(s_cast<size_t>(ai_mesh->mNumVertices));
			bi_tangents.reserve(s_cast<size_t>(ai_mesh->mNumVertices));

			// Bone weights are resized because we need to do subscript directly
			bone_ids.resize(s_cast<size_t>(ai_mesh->mNumVertices), ivec4{ 0,0,0,0 });
			bone_weights.resize(s_cast<size_t>(ai_mesh->mNumVertices), vec4{ 0,0,0,0 });

			indices.reserve(s_cast<size_t>(ai_mesh->mNumFaces) * 3);

			// Initialize vertices
			const aiVector3D  zero{ 0.0f, 0.0f, 0.0f };
			for (size_t k = 0; k < ai_mesh->mNumVertices; ++k)
			{
				const aiVector3D& pos = ai_mesh->mVertices[k];
				const aiVector3D& normal = ai_mesh->mNormals[k];
				const aiVector3D& text = ai_mesh->HasTextureCoords(0) ? ai_mesh->mTextureCoords[0][k] : zero;
				aiVector3D tangent, bi_tangent;
				if (ai_mesh->HasTangentsAndBitangents())
				{
					tangent = ai_mesh->mTangents[k];
					bi_tangent = ai_mesh->mBitangents[k];
				}

				positions.emplace_back(vec3{ pos.x, pos.y, pos.z });
				normals.emplace_back(vec3{ normal.x, normal.y, normal.z });
				uvs.emplace_back(vec2{ text.x, text.y });
				tangents.emplace_back(vec3{ tangent.x, tangent.y, tangent.z });
				bi_tangents.emplace_back(vec3{ bi_tangent.x, bi_tangent.y, bi_tangent.z });


				// updateBounds(vertices.back().pos, min_pos, max_pos);
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

				const int bone_index = static_cast<int>(res->second);
				for (size_t j = 0; j < ai_bone->mNumWeights; ++j)
				{
					const float weight = ai_bone->mWeights[j].mWeight;
					const unsigned vert_id = ai_bone->mWeights[j].mVertexId;
					fbx_loader_detail::addBoneData(bone_index, weight, bone_ids[vert_id], bone_weights[vert_id]);
				}
			}

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
			{
				auto& buffer = tangents;
				attribs[attrib_index::Tangent] = std::make_pair(mesh_modder.CreateBuffer(string_view{ r_cast<const char*>(std::data(buffer)),vkn::hlp::buffer_size(buffer) }), offset_t{ 0 });
			}
			{
				auto& buffer = bi_tangents;
				attribs[attrib_index::Bitangent] = std::make_pair(mesh_modder.CreateBuffer(string_view{ r_cast<const char*>(std::data(buffer)),vkn::hlp::buffer_size(buffer) }), offset_t{ 0 });
			}
			{
				auto& buffer = bone_ids;
				attribs[attrib_index::BoneID] = std::make_pair(mesh_modder.CreateBuffer(string_view{ r_cast<const char*>(std::data(buffer)),vkn::hlp::buffer_size(buffer) }), offset_t{ 0 });
			}
			{
				auto& buffer = bone_weights;
				attribs[attrib_index::BoneWeight] = std::make_pair(mesh_modder.CreateBuffer(string_view{ r_cast<const char*>(std::data(buffer)),vkn::hlp::buffer_size(buffer) }), offset_t{ 0 });
			}

			auto& buffer = indices;
			auto index_buffer = mesh_modder.CreateBuffer(string_view{ r_cast<const char*>(std::data(buffer)),vkn::hlp::buffer_size(buffer) });

			mesh_modder.RegisterAttribs(*mesh_handle, attribs);
			mesh_modder.SetIndexBuffer32(*mesh_handle, index_buffer, s_cast<uint32_t>(indices.size()));

			// Create mesh objects
			const auto mesh_child = scene->CreateGameObject();
			mesh_child->Name(mesh_handle->Name());
			mesh_child->Transform()->SetParent(prefab_root);
			const auto mesh_renderer = mesh_child->AddComponent<SkinnedMeshRenderer>();
			mesh_renderer->mesh = RscHandle<Mesh>{ mesh_handle };
			mesh_renderer->material_instance = mat_inst;

			// Clear the buffer
			positions.clear();
			normals.clear();
			uvs.clear();
			tangents.clear();
			bi_tangents.clear();
			bone_ids.clear();
			bone_weights.clear();

			indices.clear();

			retval.Add(mesh_handle);
		}

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
		skeleton_handle->Name(bones[0]._name);

		auto& skeleton = skeleton_handle.as<anim::Skeleton>();

		skeleton = anim::Skeleton{ bones, bones_table };

		// Setting the skeleton transform - we multiply the normalized_mesh matrix here because the bone_transform un-does it
		const mat4 skeleton_transform = fbx_loader_detail::initMat4(ai_scene->mRootNode->mTransformation).inverse();
		skeleton.SetSkeletonTransform(skeleton_transform);
		retval.Add(skeleton_handle);

		const auto animator = prefab_root->AddComponent<Animator>();
		animator->SetSkeleton(skeleton_handle);

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
			fbx_loader_detail::initAnimNodes(assimp_node_vec, ai_scene->mAnimations[i], anim_clip);
			retval.Add(anim_clip_handle);

			animator->AddAnimation(anim_clip_handle);
		}
		// animator->Play(0);
		// Saving the prefab
		const auto prefab_handle = PrefabUtility::Save(prefab_root, string{ "/assets/prefabs/" } +path_to_resource.GetStem().data() + ".idp");
		retval.Add(prefab_handle);

		scene->DestroyGameObject(prefab_root);
		return retval;
	}
	VulkanFBXLoader::VulkanFBXLoader() :_data{ std::make_unique<Data>() }
	{
	}

	VulkanFBXLoader::~VulkanFBXLoader() = default;

}
