#include <scene/Scene.inl>
#include "AssimpCompiler_helpers.h"
#include "AssimpCompiler.h"
#include <math/matrix_decomposition.inl>
#include <deque>
#include <iostream>
#include <../idk/file/FileSystem.h>
#include <math/ritters.h>
#include <ds/span.inl>


namespace idk::ai_helpers
{

	bool Scene::Import(const fs::path& fs_path)
	{
		// Note to self: Try JoinIdenticalVertices see if there's a diff. Might be faster.
		ai_scene = importer.ReadFile(fs_path.generic_string(),
			aiProcess_Triangulate |		// Triangulates non-triangles
			aiProcess_GenSmoothNormals |		// Generates missing normals
			aiProcess_CalcTangentSpace |		// Generate tangents and bi-tangents
			aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_FindInstances | aiProcess_ImproveCacheLocality);				// UVs flip	

		if (ai_scene)
		{
			has_meshes = ai_scene->HasMeshes();
			has_animation = ai_scene->HasAnimations();
		}
		
		file_ext = fs_path.extension().generic_string();
		return ai_scene != nullptr;
	}

	void Scene::CollectMeshes(aiNode* node)
	{
		node;
		num_meshes = ai_scene->mNumMeshes;
		for (size_t i = 0; i < num_meshes; ++i)
		{
			aiMesh* curr_mesh = ai_scene->mMeshes[i];
			if (mesh_table.find(curr_mesh->mName.data) == mesh_table.end())
			{
				meshes.emplace_back(curr_mesh);
				mesh_table.emplace(curr_mesh->mName.data, curr_mesh);
			}
			else
			{
				LogWarning(string{ "Mesh " } +curr_mesh->mName.data + " referenced more than once. Appending \"- Copy\".)");
				aiString new_name = { curr_mesh->mName };
				new_name.Append("- Copy");
				curr_mesh->mName = new_name;
				meshes.emplace_back(ai_scene->mMeshes[i]);
				mesh_table.emplace(curr_mesh->mName.data, ai_scene->mMeshes[i]);
			}

		}

		// vector<aiNode*> test_vec;
		// std::function<void(aiNode * node)> func;
		// func = [&test_vec , &func](aiNode* node)
		// {
		// 	if (node->mNumMeshes)
		// 		test_vec.push_back(node);
		// 
		// 	for (size_t i = 0; i < node->mNumChildren; ++i)
		// 		func(node->mChildren[i]);
		// 
		// };
		// func(scene.ai_scene->mRootNode);
		// test_vec;

		// Keep for now. Might need to account for scaled meshes in the future
		// for (size_t i = 0; i < node->mNumMeshes; ++i)
		// {
		// 	unsigned mesh_index = node->mMeshes[i];
		// 	aiMesh* curr_mesh = scene.ai_scene->mMeshes[mesh_index];
		// 
		// 	if (scene.mesh_table.find(curr_mesh->mName.data) != scene.mesh_table.end())
		// 		PrintError(string{ "Mesh " } + curr_mesh->mName.data + " referenced more than once.");
		// 	else
		// 		scene.mesh_table.emplace(curr_mesh->mName.data, curr_mesh);
		// 
		// 	// We make sure its 1-1... even if we push multiple of the same mesh aiNode
		// 	scene.meshes.emplace_back(curr_mesh);
		// 	scene.mesh_nodes.push_back(node);
		// 
		// 	scene.has_meshes = true;
		// }
		// 
		// for (size_t i = 0; i < node->mNumChildren; ++i)
		// 	CompileMeshes(scene, node->mChildren[i]);
	}

	vector<MeshData> Scene::BuildMeshBuffers() const
	{
		vector<MeshData> ret_val;
		if (!has_meshes)
			return ret_val;
		const size_t num_meshes = meshes.size();
		ret_val.resize(num_meshes);
		for (size_t i = 0; i < num_meshes; ++i)
		{
			auto& ai_mesh = meshes[i];
			auto& curr_buffer = ret_val[i];

			curr_buffer.name = ai_mesh->mName.data;

			// Allocating space in the buffers
			curr_buffer.positions.reserve(s_cast<size_t>(ai_mesh->mNumVertices));
			curr_buffer.normals.reserve(s_cast<size_t>(ai_mesh->mNumVertices));
			curr_buffer.uvs.reserve(s_cast<size_t>(ai_mesh->mNumVertices));
			curr_buffer.tangents.reserve(s_cast<size_t>(ai_mesh->mNumVertices));
			curr_buffer.bi_tangents.reserve(s_cast<size_t>(ai_mesh->mNumVertices));

			// Bone weights are resized because we need to do subscript directly
			curr_buffer.bone_ids.resize(s_cast<size_t>(ai_mesh->mNumVertices), ivec4{ 0,0,0,0 });
			curr_buffer.bone_weights.resize(s_cast<size_t>(ai_mesh->mNumVertices), vec4{ 0,0,0,0 });

			curr_buffer.indices.reserve(s_cast<size_t>(ai_mesh->mNumFaces) * 3);

			const float normalize_scale = file_ext == ".fbx" ? 0.01f : 1.0f;

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

				curr_buffer.positions.emplace_back(vec3{ pos.x, pos.y, pos.z } *normalize_scale);
				curr_buffer.normals.emplace_back(vec3{ normal.x, normal.y, normal.z });
				curr_buffer.uvs.emplace_back(vec2{ text.x, text.y });
				curr_buffer.tangents.emplace_back(vec3{ tangent.x, tangent.y, tangent.z });
				curr_buffer.bi_tangents.emplace_back(vec3{ bi_tangent.x, bi_tangent.y, bi_tangent.z });
				// updateBounds(vertices.back().pos, min_pos, max_pos);
			}

			// Initialize indices
			for (size_t k = 0; k < ai_mesh->mNumFaces; k++)
			{
				const aiFace& face = ai_mesh->mFaces[k];
				assert(face.mNumIndices == 3);
				curr_buffer.indices.push_back(face.mIndices[0]);
				curr_buffer.indices.push_back(face.mIndices[1]);
				curr_buffer.indices.push_back(face.mIndices[2]);
			}

			// Bone weights
			for (size_t k = 0; k < ai_mesh->mNumBones; ++k)
			{
				const aiBone* ai_bone = ai_mesh->mBones[k];
				auto res = final_skeleton_table.find(ai_bone->mName.data);
				assert(res != final_skeleton_table.end());

				const int bone_index = static_cast<int>(res->second);
				for (size_t j = 0; j < ai_bone->mNumWeights; ++j)
				{
					const float weight = ai_bone->mWeights[j].mWeight;
					const unsigned vert_id = ai_bone->mWeights[j].mVertexId;
					ai_helpers::AddBoneData(bone_index, weight, curr_buffer.bone_ids[vert_id], curr_buffer.bone_weights[vert_id]);
				}
			}
		}
	
		return ret_val;
	}

	CompiledMesh ai_helpers::Scene::CompileMesh(const MeshData& buffer) const
	{
		CompiledMesh mesh;
		if (!buffer.positions.empty())
		{
			vtx::Descriptor descriptor{
				.attrib = vtx::Attrib::Position,
				.stride = sizeof(vec3),
				.offset = 0,
			};

			CompiledBuffer buf{
				{descriptor},
				string{r_cast<const char*>(buffer.positions.data()), r_cast<const char*>(buffer.positions.data() + buffer.positions.size())}
			};
			mesh.buffers.emplace_back(buf);
		}

		if (!buffer.normals.empty())
		{
			vtx::Descriptor descriptor{
				.attrib = vtx::Attrib::Normal,
				.stride = sizeof(vec3),
				.offset = 0,
			};

			CompiledBuffer buf{
				{descriptor},
				string{r_cast<const char*>(buffer.normals.data()), r_cast<const char*>(buffer.normals.data() + buffer.normals.size())}
			};

			mesh.buffers.emplace_back(buf);
		}

		if (!buffer.uvs.empty())
		{
			vtx::Descriptor descriptor{
				.attrib = vtx::Attrib::UV,
				.stride = sizeof(vec2),
				.offset = 0,
			};

			CompiledBuffer buf{
				{descriptor},
				string{r_cast<const char*>(buffer.uvs.data()), r_cast<const char*>(buffer.uvs.data() + buffer.uvs.size())}
			};

			mesh.buffers.emplace_back(buf);
		}

		if (!buffer.tangents.empty())
		{
			vtx::Descriptor descriptor{
				.attrib = vtx::Attrib::Tangent,
				.stride = sizeof(vec3),
				.offset = 0,
			};


			CompiledBuffer buf{
				{descriptor},
				string{r_cast<const char*>(buffer.tangents.data()), r_cast<const char*>(buffer.tangents.data() + buffer.tangents.size())}
			};

			mesh.buffers.emplace_back(buf);
		}

		if (!buffer.bi_tangents.empty())
		{
			vtx::Descriptor descriptor{
				.attrib = vtx::Attrib::Bitangent,
				.stride = sizeof(vec3),
				.offset = 0,
			};

			CompiledBuffer buf{
				{descriptor},
				string{r_cast<const char*>(buffer.bi_tangents.data()), r_cast<const char*>(buffer.bi_tangents.data() + buffer.bi_tangents.size())}
			};

			mesh.buffers.emplace_back(buf);
		}

		if (!buffer.bone_ids.empty())
		{
			vtx::Descriptor descriptor{
				.attrib = vtx::Attrib::BoneID,
				.stride = sizeof(ivec4),
				.offset = 0,
			};

			CompiledBuffer buf{
							{descriptor},
							string{r_cast<const char*>(buffer.bone_ids.data()), r_cast<const char*>(buffer.bone_ids.data() + buffer.bone_ids.size())}
			};

			mesh.buffers.emplace_back(buf);
		}

		if (!buffer.bone_weights.empty())
		{
			vtx::Descriptor descriptor{
				.attrib = vtx::Attrib::BoneWeight,
				.stride = sizeof(vec4),
				.offset = 0,
			};

			CompiledBuffer buf{
				{descriptor},
				string{r_cast<const char*>(buffer.bone_weights.data()), r_cast<const char*>(buffer.bone_weights.data() + buffer.bone_weights.size())}
			};

			mesh.buffers.emplace_back(buf);
		}

		mesh.element_buffer = std::move(buffer.indices);

		// Compute ritters bounding volume
		span<const vec3> pos{ buffer.positions };
		mesh.bounding_volume = ritters(pos);
		
		return mesh;;
	}

#pragma region Compiling/Building Skeleton
	void Scene::CollectBones()
	{
		// Root bones must be denoted by the root bone keyword.
		// Assimp only imports bones from maya as aiBones if they have weights.
		// Generally, all bones that require animation MUST have at least one vertex weight. 
		// Example is in YY's rig where COG is to be animated but it does not have any weights. 
		for (size_t i = 0; i < ai_scene->mNumMeshes; ++i)
		{
			const aiMesh* curr_mesh = ai_scene->mMeshes[i];

			for (size_t bone_index = 0; bone_index < curr_mesh->mNumBones; ++bone_index)
			{
				aiBone* ai_bone = curr_mesh->mBones[bone_index];
				string bone_name = ai_bone->mName.data;

				if (bone_table.find(bone_name) == bone_table.end())
				{
					bone_table.emplace(bone_name, ai_bone);
					has_skeleton = true;
					has_skinned_meshes = true;
				}
			}// end per mesh bone loop
		}

		// Find the skeleton root
		if (has_skeleton)
		{
			bone_root = FindFirstNodeContains(Scene::root_bone_keyword.data(), ai_scene->mRootNode);
			if (bone_root == nullptr || bone_root == ai_scene->mRootNode)
				LogWarning(string{ "Unable to find skeleton root bone. Make sure root of skeleton contains keyword: " } + Scene::root_bone_keyword.data());
		}
	}

	void Scene::BuildSkeleton()
	{
		// Compile the pivotless bone tree
		struct BoneTreeNode
		{
			aiNode* node = nullptr;
			int parent = -1;
			mat4 parent_local_transform;
		};

		if (!has_skeleton)
			return;

		std::deque<BoneTreeNode> bone_queue;

		// Optionally check for pivots here
		bone_queue.push_back(BoneTreeNode{ bone_root, -1 });
		while (!bone_queue.empty())
		{
			auto curr_node = bone_queue.front();
			bone_queue.pop_front();
			string node_name = curr_node.node->mName.data;

			const mat4 local_transform = curr_node.parent_local_transform * to_mat4(curr_node.node->mTransformation);
			// Ignore all $assimp$ nodes in the final skeleton. We also ignore user specified excluded nodes. Everything else we will consider it a bone.
			if (node_name.find(AssimpPrefix) != string::npos || node_name.find(Scene::bone_exclude_keyword) != string::npos)
			{
				for (size_t i = 0; i < curr_node.node->mNumChildren; ++i)
				{
					bone_queue.push_front(BoneTreeNode{ curr_node.node->mChildren[i], curr_node.parent, local_transform });
				}

				continue;
			}

			anim::BoneData new_bone;
			new_bone.name = curr_node.node->mName.data;
			new_bone.parent = curr_node.parent;

			// Pre/Post rotations
			new_bone.pre_rotation = GetPreRotations(curr_node.node);
			new_bone.post_rotation = GetPostRotations(curr_node.node);

			// World
			aiMatrix4x4		ai_child_world_bind_pose;// = initMat4(curr_node.assimp_node->global_inverse_bind_pose).Inverse();
			aiVector3D		child_world_pos;
			aiVector3D		child_world_scale;
			aiQuaternion	child_world_rot;

			// Local
			aiMatrix4x4		ai_child_local_bind_pose;// = b.parent >= 0 ? initMat4(bones_out[b.parent].global_inverse_bind_pose) * ai_child_world_bind_pose : ai_child_world_bind_pose;
			aiVector3D		child_local_pos;
			aiVector3D		child_local_scale;
			aiQuaternion	child_local_rot;

			auto bone_node = bone_table.find(node_name);
			// If we can't find the aiBone, there are a couple of possiblities.
			// 1) It is a mesh that is parented to the parent bone/mesh
			// 2) It is a bone that has no weights. In which case, we will use the node transforms as the base from which we build its local transform.
			if (bone_node == bone_table.end())
			{
				// Error handling
				string error_string = "No aiBone node " + node_name + " found in bone hierarchy.";
				if (curr_node.node->mMeshes != nullptr)
					error_string += " Meshes that are parented to bones are not supported yet.";
				else
					error_string += " Using compiled transform as local pose.";
				LogWarning(error_string);

				aiMatrix4x4 global_inverse = to_aiMat4(local_transform);
				global_inverse.Inverse();

				// Multiply the parent's inverse if its there
				if (new_bone.parent >= 0)
				{
					// My global inverse is P(inv) * C_local(inv)
					global_inverse = to_aiMat4(final_skeleton[new_bone.parent].global_inverse_bind_pose) * global_inverse;
				}

				// Initialize child local/global pose
				ai_child_local_bind_pose = to_aiMat4(local_transform);
				ai_child_world_bind_pose = global_inverse;
				// ai_child_world_bind_pose.Inverse();

				// Initialize global inverse in bone
				new_bone.global_inverse_bind_pose = to_mat4(global_inverse);
			}
			else
			{
				// Initialize child local/global pose
				ai_child_world_bind_pose = bone_node->second->mOffsetMatrix;
				ai_child_world_bind_pose.Inverse();
				ai_child_local_bind_pose = new_bone.parent >= 0 ? to_aiMat4(final_skeleton[new_bone.parent].global_inverse_bind_pose) * ai_child_world_bind_pose : ai_child_world_bind_pose;

				// Initialize global inverse in bone
				new_bone.global_inverse_bind_pose = to_mat4(bone_node->second->mOffsetMatrix);
			}

			// World
			// aiDecomposeMatrix(&ai_child_world_bind_pose, &child_world_scale, &child_world_rot, &child_world_pos);

			// local
			aiDecomposeMatrix(&ai_child_local_bind_pose, &child_local_scale, &child_local_rot, &child_local_pos);

			// Setting local bind pose of both the new bone and the aiNode
			new_bone.local_bind_pose.position = to_vec3(child_local_pos);
			new_bone.local_bind_pose.rotation = to_quat(child_local_rot);
			new_bone.local_bind_pose.scale = to_vec3(child_local_scale);

			curr_node.node->mTransformation = ai_child_local_bind_pose;

			final_skeleton.emplace_back(new_bone);
			final_skeleton_table.emplace(final_skeleton.back().name, final_skeleton.size() - 1);

			for (size_t i = 0; i < curr_node.node->mNumChildren; ++i)
			{
				bone_queue.push_back(BoneTreeNode{ curr_node.node->mChildren[i], static_cast<int>(final_skeleton.size() - 1), mat4{}, });
			}
		}

		for (auto& bone : final_skeleton)
		{
			bone.global_inverse_bind_pose[3] *= 0.01f;
			bone.global_inverse_bind_pose[3].w = 1.0f;

			bone.local_bind_pose.position *= 0.01f;
		}
	}

	void Scene::BuildSkinlessSkeleton()
	{
		// Search for the root bone
		bone_root = FindFirstNodeContains(Scene::root_bone_keyword.data(), ai_scene->mRootNode);
		if (bone_root == nullptr || bone_root == ai_scene->mRootNode)
			LogWarning(string{ "Unable to find skeleton root bone. Make sure root of skeleton contains keyword: " } + Scene::root_bone_keyword.data());

		// Compile the pivotless bone tree
		struct BoneTreeNode
		{
			aiNode* node = nullptr;
			int parent = -1;
			mat4 compiled_transform;
		};


		std::deque<BoneTreeNode> bone_queue;

		// Optionally check for pivots here
		bone_queue.push_back(BoneTreeNode{ bone_root, -1 });
		while (!bone_queue.empty())
		{
			auto curr_node = bone_queue.front();
			bone_queue.pop_front();
			string node_name = curr_node.node->mName.data;

			const mat4 node_transform = curr_node.compiled_transform * to_mat4(curr_node.node->mTransformation);

			// Ignore all $assimp$ nodes in the final skeleton. We also ignore user specified excluded nodes. Everything else we will consider it a bone.
			if (node_name.find(AssimpPrefix) != string::npos || node_name.find(Scene::bone_exclude_keyword) != string::npos)
			{
				for (size_t i = 0; i < curr_node.node->mNumChildren; ++i)
				{
					bone_queue.push_front(BoneTreeNode{ curr_node.node->mChildren[i], curr_node.parent, node_transform });
				}

				continue;
			}

			anim::BoneData new_bone;
			new_bone.name = curr_node.node->mName.data;
			new_bone.parent = curr_node.parent;

			// World
			aiMatrix4x4		ai_child_world_bind_pose;// = initMat4(curr_node.assimp_node->global_inverse_bind_pose).Inverse();
			aiVector3D		child_world_pos;
			aiVector3D		child_world_scale;
			aiQuaternion	child_world_rot;

			// Local
			aiMatrix4x4		ai_child_local_bind_pose;// = b.parent >= 0 ? initMat4(bones_out[b.parent].global_inverse_bind_pose) * ai_child_world_bind_pose : ai_child_world_bind_pose;
			aiVector3D		child_local_pos;
			aiVector3D		child_local_scale;
			aiQuaternion	child_local_rot;

			auto bone_node = bone_table.find(node_name);
			aiMatrix4x4 global_inverse = curr_node.node->mTransformation;
			global_inverse.Inverse();

			// Multiply the parent's inverse if its there
			if (new_bone.parent >= 0)
			{
				// My global inverse is P(inv) * C_local(inv)
				global_inverse = to_aiMat4(skinless_skeleton[new_bone.parent].global_inverse_bind_pose) * global_inverse;
			}

			// Initialize child local/global pose
			ai_child_local_bind_pose = to_aiMat4(node_transform);
			ai_child_world_bind_pose = global_inverse;
			ai_child_world_bind_pose.Inverse();

			// Initialize global inverse in bone
			new_bone.global_inverse_bind_pose = to_mat4(global_inverse);
			new_bone.global_inverse_bind_pose[3] /= 100.0f;
			new_bone.global_inverse_bind_pose[3].w = 1.0f;

			// World
			// aiDecomposeMatrix(&ai_child_world_bind_pose, &child_world_scale, &child_world_rot, &child_world_pos);

			// local
			aiDecomposeMatrix(&ai_child_local_bind_pose, &child_local_scale, &child_local_rot, &child_local_pos);

			// Setting local bind pose of both the new bone and the aiNode
			new_bone.local_bind_pose.position = to_vec3(child_local_pos) / 100.0f;
			new_bone.local_bind_pose.rotation = to_quat(child_local_rot);
			new_bone.local_bind_pose.scale = to_vec3(child_local_scale);

			curr_node.node->mTransformation = ai_child_local_bind_pose;

			skinless_skeleton.emplace_back(new_bone);
			skinless_skeleton_table.emplace(skinless_skeleton.back().name, skinless_skeleton.size() - 1);

			for (size_t i = 0; i < curr_node.node->mNumChildren; ++i)
			{
				bone_queue.push_back(BoneTreeNode{ curr_node.node->mChildren[i], static_cast<int>(skinless_skeleton.size() - 1), mat4{} });
			}
		}

		for (auto& bone : skinless_skeleton)
		{
			bone.global_inverse_bind_pose[3] *= 0.01f;
			bone.global_inverse_bind_pose[3].w = 1.0f;

			bone.local_bind_pose.position *= 0.01f;
		}
	}

	anim::Skeleton Scene::CompileSkeleton() const
	{
		anim::Skeleton ret_val{ final_skeleton, final_skeleton_table };
		if (!final_skeleton.empty())
			ret_val.Name(final_skeleton[0].name);
		return ret_val;
	}

#pragma endregion

#pragma region Compiling/Building Animations
	vector<AnimationData> Scene::CollectAndBuildAnimations()
	{
		// Generate anim map.
		// Compile all animated channels.
		// Error checking also happens here.
		vector<AnimationData> ret_val;

		// We create a skinless skeleton if the scene has animations but no mesh
		if (ai_scene->HasAnimations() && !has_skeleton)
		{
			BuildSkinlessSkeleton();
		}

		for (size_t i = 0; i < ai_scene->mNumAnimations; ++i)
		{
			aiAnimation* ai_anim = ai_scene->mAnimations[i];
			AnimationData compiled_clip;

			compiled_clip.name = ai_anim->mName.data;
			compiled_clip.fps = static_cast<float>(ai_anim->mTicksPerSecond <= 0.0 ? 24.0f : ai_anim->mTicksPerSecond);
			compiled_clip.num_ticks = static_cast<float>(ai_anim->mDuration);
			compiled_clip.duration = compiled_clip.num_ticks / compiled_clip.fps;

			for (size_t k = 0; k < ai_anim->mNumChannels; ++k)
			{
				aiNodeAnim* ai_channel = ai_anim->mChannels[k];
				ChannelType channel_type = None;

				string channel_bone_name = ai_channel->mNodeName.data;
				size_t pos = channel_bone_name.find(AssimpPrefix);
				if (pos != string::npos)
				{
					// Get the type of the channel
					string assimp_suffix = channel_bone_name.substr(pos + AssimpPrefix.length());
					if (assimp_suffix == "Translation")
						channel_type = Translate;
					else if (assimp_suffix == "Rotation")
						channel_type = Rotate;
					else if (assimp_suffix == "Scaling")
						channel_type = Scale;
					else // Error
					{
						string error_string = "[Error] Unrecognized Channel Node " + channel_bone_name + " found. Skipping this channel.";
						LogWarning(error_string);
						continue;
					}

					// Get the actual bone name
					channel_bone_name = channel_bone_name.substr(0, pos);
				}
				else
				{
					channel_type = Bone;
				}

				// Check if the current bone is done already or not.
				size_t animated_bone_index = 0;
				auto found_animated_bone = compiled_clip.animated_bone_table.find(channel_bone_name);
				if (found_animated_bone == compiled_clip.animated_bone_table.end())
				{
					// Doesn't exists yet so we create a new animated bone and put it in.
					anim::AnimatedBone anim_bone;
					anim_bone.bone_name = channel_bone_name;

					animated_bone_index = compiled_clip.animated_bones.size();
					compiled_clip.animated_bone_table.emplace(channel_bone_name, animated_bone_index);
					compiled_clip.animated_bones.emplace_back(anim_bone);
				}
				else
					animated_bone_index = found_animated_bone->second;

				// Get the actual animated bone
				auto& animated_bone = compiled_clip.animated_bones[animated_bone_index];

				// Error handling: Check if the bone has keys in it already for the specified type of animation
				// This is done in each specified function
				switch (channel_type)
				{
				case Translate:
					BuildTranslateChannel(animated_bone, ai_channel);
					break;
				case Rotate:
					BuildRotateChannel(animated_bone, ai_channel);
					break;
				case Scale:
					BuildScaleChannel(animated_bone, ai_channel);
					break;
				case Bone:
					BuildBoneChannel(animated_bone, ai_channel);
					break;
				case None:
					break;
				default:
					break;
				}

			}
			BuildAnimation(compiled_clip);
			ai_anim_clips.emplace_back(ai_anim);
			ret_val.emplace_back(compiled_clip);

			has_animation = true;
		}
		
		return ret_val;
	}

	void Scene::BuildTranslateChannel(anim::AnimatedBone& anim_bone, aiNodeAnim* anim_channel)
	{
		// Error handling: There should not be any keys in the scale or rotate section
		if (anim_channel->mNumRotationKeys > 1)
			LogWarning(std::to_string(anim_channel->mNumRotationKeys) +
				" Rotation keys detected inside a translation channel(" + anim_channel->mNodeName.data + "). Ignoring these keys.");
		if (anim_channel->mNumScalingKeys > 1)
			LogWarning(std::to_string(anim_channel->mNumScalingKeys) +
				" Scaling keys detected inside a translation channel(" + anim_channel->mNodeName.data + "). Ignoring these keys.");
		if (!anim_bone.translate_track.empty())
		{
			LogWarning(+
				"Animated bone " + anim_bone.bone_name + " has more than 1 channel that has translation keys. Ignoring the one with fewer keys.");

			// Don't put in the keys if the current track has more keys that this channel
			if (anim_bone.translate_track.size() > anim_channel->mNumPositionKeys)
				return;
		}

		// For translation, there is no pre or post transformation so it 
		// is safe to just add all the keys into the animated bone.
		for (size_t i = 0; i < anim_channel->mNumPositionKeys; ++i)
		{
			auto& pos_key = anim_channel->mPositionKeys[i];
			anim_bone.translate_track.emplace_back(to_vec3(pos_key.mValue) * 0.01f, s_cast<float>(pos_key.mTime));
		}
	}

	void Scene::BuildRotateChannel(anim::AnimatedBone& anim_bone, aiNodeAnim* anim_channel)
	{
		// Error handling: There should not be any keys in the position or scale section
		if (anim_channel->mNumPositionKeys > 1)
			LogWarning(std::to_string(anim_channel->mNumPositionKeys) +
				" Position keys detected inside a translation channel(" + anim_channel->mNodeName.data + "). Ignoring these keys.");
		if (anim_channel->mNumScalingKeys > 1)
			LogWarning(std::to_string(anim_channel->mNumScalingKeys) +
				" Scaling keys detected inside a translation channel(" + anim_channel->mNodeName.data + "). Ignoring these keys.");
		if (!anim_bone.rotation_track.empty())
		{
			LogWarning("Animated bone " + anim_bone.bone_name + " has more than 1 channel that has rotation keys. Ignoring the one with fewer keys.");

			// Don't put in the keys if the current track has more keys that this channel
			if (anim_bone.rotation_track.size() > anim_channel->mNumRotationKeys)
				return;
		}

		// aiNode* node = scene.bone_root->FindNode(anim_channel->mNodeName.data);
		// 
		// // Need to find all the chained rotations first
		// aiMatrix4x4 pre_rotation;
		// aiIdentityMatrix4(&pre_rotation);
		// aiMatrix4x4 post_rotation;
		// aiIdentityMatrix4(&post_rotation);
		// 
		// // Rotation Order: OFFSET_ROT * PIVOT_ROT *  PRE_ROT * !!ROTATION(KEYFRAME)!! * POST_ROT * PIVOT_INV_ROT 
		// 
		// // Find all the pre transformations. 
		// // From the base node, traverse up and check for the pre rotations
		// {
		// 	aiNode* parent = node->mParent;
		// 	for (int i = 0; i < PreRotationMax; ++i)
		// 	{
		// 		if (parent->mName.data == anim_bone.bone_name + AssimpPrefix + PreRotateSuffix[i])
		// 		{
		// 			pre_rotation = pre_rotation * parent->mTransformation;
		// 			parent = parent->mParent;
		// 		}
		// 	}
		// }
		// // Find all post transforms.
		// {
		// 	aiNode* child = node->mChildren[0];
		// 	for (int i = 0; i < PostRotationMax; ++i)
		// 	{
		// 		if (child->mName.data == anim_bone.bone_name + AssimpPrefix + PostRotateSuffix[i])
		// 		{
		// 			post_rotation = post_rotation * child->mTransformation;
		// 			child = child->mChildren[0];
		// 		}
		// 	}
		// }
		// 
		// // Pre rotate decompose
		// aiVector3D pre_pos;
		// aiQuaternion pre_rot;
		// aiVector3D pre_scale;
		// aiDecomposeMatrix(&pre_rotation, &pre_scale, &pre_rot, &pre_pos);
		// 
		// // Post rotate decompose
		// aiVector3D post_pos;
		// aiQuaternion post_rot;
		// aiVector3D post_scale;
		// aiDecomposeMatrix(&post_rotation, &post_scale, &post_rot, &post_pos);
		// 
		// quat pre_quat = to_quat(pre_rot);
		// quat post_quat = to_quat(post_rot);
		// 
		// // Error handling : Scale and pos should not be affected.
		// if (!pre_scale.Equal(aiVector3D{ 1,1,1 }) || !post_scale.Equal(aiVector3D{ 1,1,1 }))
		// {
		// 	PrintError("Pre rotation has scale for " + anim_bone.bone_name);
		// }
		// 
		// if (!pre_pos.Equal(aiVector3D{}) || !post_pos.Equal(aiVector3D{ }))
		// {
		// 	PrintError("Post rotation has translation for " + anim_bone.bone_name);
		// }

		for (size_t i = 0; i < anim_channel->mNumRotationKeys; ++i)
		{
			aiQuatKey& rot_key = anim_channel->mRotationKeys[i];
			quat rot_val = to_quat(rot_key.mValue);

			//rot_val = pre_quat * rot_val * post_quat;
			anim_bone.rotation_track.emplace_back(rot_val, s_cast<float>(rot_key.mTime));
		}
	}

	void Scene::BuildScaleChannel(anim::AnimatedBone& anim_bone, aiNodeAnim* anim_channel)
	{
		// Error handling: There should not be any keys in the position or scale section
		if (anim_channel->mNumPositionKeys > 1)
			LogWarning(std::to_string(anim_channel->mNumPositionKeys) +
				" Position keys detected inside a translation channel(" + anim_channel->mNodeName.data + "). Ignoring these keys.");
		if (anim_channel->mNumRotationKeys > 1)
			LogWarning(std::to_string(anim_channel->mNumRotationKeys) +
				" Rotation keys detected inside a translation channel(" + anim_channel->mNodeName.data + "). Ignoring these keys.");
		if (!anim_bone.scale_track.empty())
		{
			LogWarning("Animated bone " + anim_bone.bone_name + " has more than 1 channel that has scaling keys. Ignoring the one with fewer keys.");

			// Don't put in the keys if the current track has more keys that this channel
			if (anim_bone.scale_track.size() > anim_channel->mNumScalingKeys)
				return;
		}

		aiNode* node = bone_root->FindNode(anim_channel->mNodeName.data);

		// Need to find all the chained rotations first
		aiMatrix4x4 pre_scale;
		aiIdentityMatrix4(&pre_scale);

		// Scaling Order: OFFSET_SCALE * PIVOT_SCALE *  !!SCALE(KEYFRAME)!!

		// Find all the pre transformations. 
		// From the base node, traverse up and check for the pre rotations
		aiNode* parent = node->mParent;
		for (int i = 0; i < PreScaleMax; ++i)
		{
			if (parent->mName.data == anim_bone.bone_name + AssimpPrefix.data() + PreScaleSuffix[i].data())
			{
				pre_scale = pre_scale * parent->mTransformation;
				parent = parent->mParent;
			}
		}

		aiVector3D pos;
		aiQuaternion rot;
		aiVector3D scale;
		aiMatrix4x4 anim_mat;
		aiIdentityMatrix4(&anim_mat);
		for (size_t i = 0; i < anim_channel->mNumScalingKeys; ++i)
		{
			aiVectorKey& scale_key = anim_channel->mScalingKeys[i];
			aiMatrix4x4::Scaling(scale_key.mValue, anim_mat);

			aiMatrix4x4 scale_mat = pre_scale * anim_mat;

			scale_mat.Decompose(scale, rot, pos);

			// Error handling: Scale and pos should not be affected.
			if (!rot.Equal(aiQuaternion{}))
			{
				LogWarning("Result of scale key with pre scale has rotation factors.");
			}

			if (!pos.Equal(aiVector3D{}))
			{
				LogWarning("Result of scale key with pre/post rotation has translation factors.");
			}

			anim_bone.scale_track.emplace_back(to_vec3(scale), s_cast<float>(scale_key.mTime));
		}
	}

	void Scene::BuildBoneChannel(anim::AnimatedBone& anim_bone, aiNodeAnim* anim_channel)
	{
		if (!anim_bone.scale_track.empty())
		{
			LogWarning("Animated bone " + anim_bone.bone_name + " has more than 1 channel that has scaling keys. Ignoring the one with fewer keys.");

			// Don't put in the keys if the current track has more keys that this channel
			if (anim_bone.scale_track.size() > anim_channel->mNumScalingKeys)
				return;
		}

		if (!anim_bone.rotation_track.empty())
		{
			LogWarning("Animated bone " + anim_bone.bone_name + " has more than 1 channel that has rotation keys. Ignoring the one with fewer keys.");

			// Don't put in the keys if the current track has more keys that this channel
			if (anim_bone.rotation_track.size() > anim_channel->mNumRotationKeys)
				return;
		}

		if (!anim_bone.translate_track.empty())
		{
			LogWarning("Animated bone " + anim_bone.bone_name + " has more than 1 channel that has translation keys. Ignoring the one with fewer keys.");

			// Don't put in the keys if the current track has more keys that this channel
			if (anim_bone.translate_track.size() > anim_channel->mNumPositionKeys)
				return;
		}

		// For bone, we just put all the keys in if we can.
		for (size_t i = 0; i < anim_channel->mNumPositionKeys; ++i)
		{
			auto& pos_key = anim_channel->mPositionKeys[i];
			anim_bone.translate_track.emplace_back(to_vec3(pos_key.mValue) * 0.01f, s_cast<float>(pos_key.mTime));
		}

		for (size_t i = 0; i < anim_channel->mNumRotationKeys; ++i)
		{
			auto& rot_key = anim_channel->mRotationKeys[i];
			anim_bone.rotation_track.emplace_back(to_quat(rot_key.mValue), s_cast<float>(rot_key.mTime));
		}

		for (size_t i = 0; i < anim_channel->mNumScalingKeys; ++i)
		{
			auto& scale_key = anim_channel->mScalingKeys[i];
			anim_bone.scale_track.emplace_back(to_vec3(scale_key.mValue), s_cast<float>(scale_key.mTime));
		}
	}

	void Scene::BuildAnimation(AnimationData& anim_data) const
	{
		// Do all the keyframe optimizations here.
		const auto trans_pred = [](const anim::KeyFrame<vec3>& lhs, const anim::KeyFrame<vec3>& rhs)
		{
			return vec3_equal(lhs.val, rhs.val);
		};

		const auto scale_pred = [](const anim::KeyFrame<vec3>& lhs, const anim::KeyFrame<vec3>& rhs)
		{
			return vec3_equal(lhs.val, rhs.val, 0.01f);
		};

		for (auto& anim_bone : anim_data.animated_bones)
		{
			// Translation optimization
			const anim::KeyFrame<vec3> t_back = anim_bone.translate_track.back();
			for (auto itr = anim_bone.translate_track.begin() + 1; itr != anim_bone.translate_track.end(); ++itr)
			{
				const auto next = itr + 1;
				if (next != anim_bone.translate_track.end() && trans_pred(*itr, *next))
				{
					itr = anim_bone.translate_track.erase(itr);
				}
			}
			if (anim_bone.translate_track.size() == 1)
				anim_bone.translate_track.emplace_back(t_back);
			
			// Scale optimization
			const anim::KeyFrame<vec3> s_back = anim_bone.scale_track.back();
			for (auto itr = anim_bone.scale_track.begin() + 1; itr != anim_bone.scale_track.end(); ++itr)
			{
				const auto next = itr + 1;
				if (next != anim_bone.scale_track.end() && scale_pred(*itr, *next))
				{
					itr = anim_bone.scale_track.erase(itr);
				}
			}
			if (anim_bone.scale_track.size() == 1)
				anim_bone.scale_track.emplace_back(s_back);
		}
	}
	vector<anim::Animation> Scene::CompileAnimations()
	{
		auto animations{ CollectAndBuildAnimations() };

		vector<anim::Animation> ret_val;
		ret_val.reserve(animations.size());
		
		for (auto& anim_data : animations)
		{
			anim::Animation anim_clip;

			anim_clip.Name(anim_data.name);
			anim_clip.SetSpeeds(anim_data.fps, anim_data.duration, anim_data.num_ticks);

			for (auto& animated_bone : anim_data.animated_bones)
			{
				anim_clip.AddAnimatedBone(animated_bone);
			}

			ret_val.emplace_back(std::move(anim_clip));
		}

		return ret_val;
	}
#pragma endregion

	// Utility functions
	void LogWarning(const string& error)
	{
		LOG_WARNING_TO(LogPool::ANIM, error);
	}

	aiNode* FindFirstNodeContains(string name, aiNode* node)
	{
		if (string{ node->mName.data }.find(name) != string::npos)
			return node;

		// recurse
		for (size_t i = 0; i < node->mNumChildren; ++i)
		{
			auto found = FindFirstNodeContains(name, node->mChildren[i]);
			if (found != nullptr)
				return found;
		}

		return nullptr;
	}

	void DumpNodes(aiNode* node)
	{
		UNREFERENCED_PARAMETER(node);
	}

	quat GetPreRotations(const aiNode* node)
	{
		// Rotation Order: OFFSET_ROT * PIVOT_ROT *  PRE_ROT * !!ROTATION(KEYFRAME)!! * POST_ROT * PIVOT_INV_ROT 

		// Find all the pre transformations. 
		// From the base node, traverse up and check for the pre rotations
		// Need to find all the chained rotations first
		string node_name = node->mName.data;
		const aiNode* start_node = node;
		while (start_node->mParent)
		{
			string parent_name = start_node->mParent->mName.data;
			if (parent_name.find(node_name) == string::npos)
				break;
			start_node = start_node->mParent;
		}

		aiMatrix4x4 pre_rotation;
		aiIdentityMatrix4(&pre_rotation);
		int curr_pre_rot_index = 0;
		while (start_node != node)
		{
			// Check if start_node matches any of the pre rotations.
			for (int i = curr_pre_rot_index; i < PreRotationMax; ++i)
			{
				if (start_node->mName.data == node_name + AssimpPrefix.data() + PreRotateSuffix[i].data())
				{
					// Set the starting point of the next pre rotation check
					pre_rotation = pre_rotation * start_node->mTransformation;
					curr_pre_rot_index = i + 1;
					break;
				}
			}

			if (curr_pre_rot_index >= PreRotationMax)
				break;
			start_node = start_node->mChildren[0];
		}

		// Pre rotate decompose
		aiVector3D pre_pos;
		aiQuaternion pre_rot;
		aiVector3D pre_scale;
		aiDecomposeMatrix(&pre_rotation, &pre_scale, &pre_rot, &pre_pos);

		quat pre_quat = to_quat(pre_rot);

		// Error handling : Scale and pos should not be affected.
		if (!pre_scale.Equal(aiVector3D{ 1,1,1 }))
		{
			LogWarning("Pre rotation has scale for " + node_name);
		}

		if (!pre_pos.Equal(aiVector3D{}))
		{
			LogWarning("Post rotation has translation for " + node_name);
		}

		return pre_quat;
	}

	quat GetPostRotations(const aiNode* node)
	{
		// Rotation Order: OFFSET_ROT * PIVOT_ROT *  PRE_ROT * !!ROTATION(KEYFRAME)!! * POST_ROT * PIVOT_INV_ROT 

		// Find all the pre transformations. 
		// From the base node, traverse up and check for the pre rotations
		// Need to find all the chained rotations first
		string node_name = node->mName.data;
		const aiNode* start_node = node;
		while (start_node->mParent)
		{
			string parent_name = start_node->mParent->mName.data;
			if (parent_name.find(node_name) == string::npos)
				break;
			start_node = start_node->mParent;
		}

		aiMatrix4x4 post_rotation;
		aiIdentityMatrix4(&post_rotation);
		int curr_post_rot_index = 0;
		while (start_node != node)
		{
			// Check if start_node matches any of the pre rotations.
			for (int i = curr_post_rot_index; i < PostRotationMax; ++i)
			{
				if (start_node->mName.data == node_name + AssimpPrefix.data() + PostRotateSuffix[i].data())
				{
					// Set the starting point of the next pre rotation check
					post_rotation = post_rotation * start_node->mTransformation;
					curr_post_rot_index = i + 1;
					break;
				}
			}

			// If there are no more post rotations to check for we jsut break
			if (curr_post_rot_index >= PostRotationMax)
				break;
			start_node = start_node->mChildren[0];
		}

		// Post rotate decompose
		aiVector3D post_pos;
		aiQuaternion post_rot;
		aiVector3D post_scale;
		aiDecomposeMatrix(&post_rotation, &post_scale, &post_rot, &post_pos);


		quat post_quat = to_quat(post_rot);

		// Error handling : Scale and pos should not be affected.
		if (!post_scale.Equal(aiVector3D{ 1,1,1 }))
		{
			LogWarning("Post rotation has scale for " + node_name);
		}

		if (!post_pos.Equal(aiVector3D{ }))
		{
			LogWarning("Post rotation has translation for " + node_name);
		}

		return post_quat;
	}

#pragma region Conversion Helpers
	vec3 to_vec3(const aiVector3D& ai_vec)
	{
		return vec3(ai_vec.x, ai_vec.y, ai_vec.z);
	}

	quat to_quat(const aiQuaternion& ai_quat)
	{
		return quat(ai_quat.x, ai_quat.y, ai_quat.z, ai_quat.w);
	}

	mat4 to_mat4(const aiMatrix4x4& ai_mat)
	{
		return mat4(
			ai_mat.a1, ai_mat.a2, ai_mat.a3, ai_mat.a4,
			ai_mat.b1, ai_mat.b2, ai_mat.b3, ai_mat.b4,
			ai_mat.c1, ai_mat.c2, ai_mat.c3, ai_mat.c4,
			ai_mat.d1, ai_mat.d2, ai_mat.d3, ai_mat.d4
		);
	}

	mat4 to_mat4(const aiMatrix3x3& ai_mat)
	{
		return mat4(
			ai_mat.a1, ai_mat.a2, ai_mat.a3, 0.0f,
			ai_mat.b1, ai_mat.b2, ai_mat.b3, 0.0f,
			ai_mat.c1, ai_mat.c2, ai_mat.c3, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}

	aiMatrix4x4 to_aiMat4(const mat4& mat)
	{
		return aiMatrix4x4{
			mat[0][0], mat[1][0], mat[2][0], mat[3][0],
			mat[0][1], mat[1][1], mat[2][1], mat[3][1],
			mat[0][2], mat[1][2], mat[2][2], mat[3][2],
			mat[0][3], mat[1][3], mat[2][3], mat[3][3]
		};
	}

#pragma endregion 

#pragma region Comparison Helpers
	bool flt_equal(float a, float b, float eps)
	{
		return abs(a - b) < eps;
	}

	bool vec3_equal(const vec3& lhs, const vec3& rhs, float eps)
	{
		return	flt_equal(lhs[0], rhs[0], eps) &&
			flt_equal(lhs[1], rhs[1], eps) &&
			flt_equal(lhs[2], rhs[2], eps);
	}

	bool vec4_equal(const vec4& lhs, const vec4& rhs)
	{
		return	flt_equal(lhs[0], rhs[0]) &&
			flt_equal(lhs[1], rhs[1]) &&
			flt_equal(lhs[2], rhs[2]) &&
			flt_equal(lhs[3], rhs[3]);
	}

	bool quat_equal(const quat& lhs, const quat& rhs)
	{
		return	flt_equal(lhs[0], rhs[0], idk::constants::epsilon<float>()) &&
			flt_equal(lhs[1], rhs[1], idk::constants::epsilon<float>()) &&
			flt_equal(lhs[2], rhs[2], idk::constants::epsilon<float>()) &&
			flt_equal(lhs[3], rhs[3], idk::constants::epsilon<float>());
	}

	bool mat4_equal(const mat4& lhs, const mat4& rhs)
	{
		return	vec4_equal(lhs[0], rhs[0]) &&
			vec4_equal(lhs[1], rhs[1]) &&
			vec4_equal(lhs[2], rhs[2]) &&
			vec4_equal(lhs[3], rhs[3]);
	}

	void AddBoneData(unsigned id_in, float weight_in, ivec4& ids_out, vec4& weights_out)
	{
		for (unsigned i = 0; i < 4; i++)
		{
			if (weights_out[i] == 0.0)
			{
				ids_out[i] = id_in;
				weights_out[i] = weight_in;
				return;
			}
		}
		// Need to get the bone with the lowest weight and replace it with this one
		unsigned min_index = 0;
		for (unsigned i = 1; i < 4; i++)
		{
			if (weights_out[i] < weights_out[min_index])
			{
				min_index = i;
			}
		}

		if (weight_in > weights_out[min_index])
		{
			ids_out[min_index] = id_in;
			weights_out[min_index] = weight_in;
		}

		// Normalize all weights
		auto sum_weights = weights_out[0] + weights_out[1] + weights_out[2] + weights_out[3];
		weights_out /= sum_weights;
		sum_weights = (weights_out[0] + weights_out[1] + weights_out[2] + weights_out[3]);
		//assert(abs(1.0f - sum_weights) < idk::constants::epsilon<float>());
	}

#pragma endregion 
	

}