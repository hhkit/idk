#include "pch.h"
#include "AssimpImporter_helpers.h"
#include "AssimpImporter.h"
#include <math/matrix_decomposition.h>
#include <deque>
#include <iostream>
#include <idk/file/FileSystem.h>

namespace idk::ai_helpers
{

	bool Import(Scene& scene, PathHandle path_to_resource)
	{
		// Note to self: Try JoinIdenticalVertices see if there's a diff. Might be faster.
		scene.ai_scene = scene.importer.ReadFile(path_to_resource.GetFullPath().data(),
						 aiProcess_Triangulate		|		// Triangulates non-triangles
						 aiProcess_GenSmoothNormals |		// Generates missing normals
						 aiProcess_CalcTangentSpace |		// Generate tangents and bi-tangents
						 aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_FindInstances | aiProcess_ImproveCacheLocality);				// UVs flip	

		if (scene.ai_scene)
		{
			scene.has_meshes = scene.ai_scene->HasMeshes();
			scene.has_animation = scene.ai_scene->HasAnimations();
		}

		return scene.ai_scene != nullptr;
	}

	void CompileMeshes(Scene& scene, aiNode* node)
	{
		node;
		scene.num_meshes = scene.ai_scene->mNumMeshes;
		for (size_t i = 0; i < scene.num_meshes; ++i)
		{
			scene.meshes.emplace_back(scene.ai_scene->mMeshes[i]);
		}

		
		// Keep for now. Might need to account for scaled meshes in the future
		// for (size_t i = 0; i < node->mNumMeshes; ++i)
		// {
		// 	unsigned mesh_index = node->mMeshes[i];
		// 	aiMesh* curr_mesh = scene.ai_scene->mMeshes[mesh_index];
		// 
		// 	if (scene.mesh_table.find(curr_mesh->mName.data) != scene.mesh_table.end())
		// 		PrintError(string{ "[Warning] Mesh " } + curr_mesh->mName.data + " referenced more than once.");
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

#pragma region Compiling/Building Skeleton
	void CompileBones(Scene& scene)
	{
		// Root bones must be denoted by the root bone keyword.
		// Assimp only imports bones from maya as aiBones if they have weights.
		// Generally, all bones that require animation MUST have at least one vertex weight. 
		// Example is in YY's rig where COG is to be animated but it does not have any weights. 
		for (size_t i = 0; i < scene.ai_scene->mNumMeshes; ++i)
		{
			const aiMesh* curr_mesh = scene.ai_scene->mMeshes[i];

			for (size_t bone_index = 0; bone_index < curr_mesh->mNumBones; ++bone_index)
			{
				aiBone* ai_bone = curr_mesh->mBones[bone_index];
				string bone_name = ai_bone->mName.data;

				if (scene.bone_table.find(bone_name) == scene.bone_table.end())
				{
					scene.bone_table.emplace(bone_name, ai_bone);
					scene.has_skeleton = true;
					scene.has_skinned_meshes = true;
				}
			}// end per mesh bone loop
		}

		// Find the skeleton root
		if (scene.has_skeleton)
		{
			scene.bone_root = FindFirstNodeContains("_root_", scene.ai_scene->mRootNode);
			assert(scene.bone_root != scene.ai_scene->mRootNode);
			assert(scene.bone_root != nullptr);
		}
	}

	void BuildSkeleton(Scene& scene)
	{
		// Compile the pivotless bone tree
		struct BoneTreeNode
		{
			aiNode* node = nullptr;
			int parent = -1;
			mat4 compiled_transform;
		};

		if (!scene.has_skeleton)
			return;

		std::deque<BoneTreeNode> bone_queue;
		vector<anim::Bone>& final_skeleton = scene.final_skeleton;
		auto& final_skeleton_table = scene.final_skeleton_table;

		// Optionally check for pivots here
		bone_queue.push_back(BoneTreeNode{ scene.bone_root, -1 });
		while (!bone_queue.empty())
		{
			auto curr_node = bone_queue.front();
			bone_queue.pop_front();
			string node_name = curr_node.node->mName.data;

			const mat4 node_transform = curr_node.compiled_transform * to_mat4(curr_node.node->mTransformation);

			// Ignore all $assimp$ nodes in the final skeleton. We also ignore user specified excluded nodes. Everything else we will consider it a bone.
			if (node_name.find(AssimpPrefix) != string::npos || node_name.find(AssimpImporter::bone_exclude_keyword) != string::npos)
			{
				for (size_t i = 0; i < curr_node.node->mNumChildren; ++i)
				{
					bone_queue.push_front(BoneTreeNode{ curr_node.node->mChildren[i], curr_node.parent, node_transform });
				}

				continue;
			}

			anim::Bone new_bone;
			new_bone._name = curr_node.node->mName.data;
			new_bone._parent = curr_node.parent;

			// World
			aiMatrix4x4		ai_child_world_bind_pose;// = initMat4(curr_node.assimp_node->_global_inverse_bind_pose).Inverse();
			aiVector3D		child_world_pos;
			aiVector3D		child_world_scale;
			aiQuaternion	child_world_rot;

			// Local
			aiMatrix4x4		ai_child_local_bind_pose;// = b._parent >= 0 ? initMat4(bones_out[b._parent]._global_inverse_bind_pose) * ai_child_world_bind_pose : ai_child_world_bind_pose;
			aiVector3D		child_local_pos;
			aiVector3D		child_local_scale;
			aiQuaternion	child_local_rot;

			auto bone_node = scene.bone_table.find(node_name);
			// If we can't find the aiBone, there are a couple of possiblities.
			// 1) It is a mesh that is parented to the parent bone/mesh
			// 2) It is a bone that has no weights. In which case, we will use the node transforms as the base from which we build its local transform.
			if (bone_node == scene.bone_table.end())
			{
				// Error handling
				string error_string = "[Warning] None aiBone node"  + node_name + " found in bone hierarchy.";
				if (curr_node.node->mMeshes != nullptr)
					error_string += " Meshes that are parented to bones are not supported yet.";
				else
					error_string += " Using compiled transform as local pose.";
				PrintError(error_string);

				aiMatrix4x4 global_inverse = curr_node.node->mTransformation;
				global_inverse.Inverse();

				// Multiply the parent's inverse if its there
				if (new_bone._parent >= 0)
				{
					// My global inverse is P(inv) * C_local(inv)
					global_inverse = to_aiMat4(final_skeleton[new_bone._parent]._global_inverse_bind_pose) * global_inverse;
				}

				// Initialize child local/global pose
				ai_child_local_bind_pose = to_aiMat4(node_transform);
				ai_child_world_bind_pose = global_inverse;
				ai_child_world_bind_pose.Inverse();

				// Initialize global inverse in bone
				new_bone._global_inverse_bind_pose = to_mat4(global_inverse);
			}
			else
			{
				// Initialize child local/global pose
				ai_child_world_bind_pose = bone_node->second->mOffsetMatrix;
				ai_child_world_bind_pose.Inverse();
				ai_child_local_bind_pose = new_bone._parent >= 0 ? to_aiMat4(final_skeleton[new_bone._parent]._global_inverse_bind_pose) * ai_child_world_bind_pose : ai_child_world_bind_pose;

				// Initialize global inverse in bone
				new_bone._global_inverse_bind_pose = to_mat4(bone_node->second->mOffsetMatrix);
			}

			// World
			aiDecomposeMatrix(&ai_child_world_bind_pose, &child_world_scale, &child_world_rot, &child_world_pos);

			// local
			aiDecomposeMatrix(&ai_child_local_bind_pose, &child_local_scale, &child_local_rot, &child_local_pos);

			// Setting local bind pose of both the new bone and the aiNode
			new_bone._local_bind_pose.position = to_vec3(child_local_pos);
			new_bone._local_bind_pose.rotation = to_quat(child_local_rot);
			new_bone._local_bind_pose.scale = to_vec3(child_local_scale);

			curr_node.node->mTransformation = ai_child_local_bind_pose;

			final_skeleton.emplace_back(new_bone);
			final_skeleton_table.emplace(final_skeleton.back()._name, final_skeleton.size() - 1);

			for (size_t i = 0; i < curr_node.node->mNumChildren; ++i)
			{
				bone_queue.push_back(BoneTreeNode{ curr_node.node->mChildren[i], static_cast<int>(final_skeleton.size() - 1), mat4{} });
			}
		}
	}

	void BuildSkinlessSkeleton(Scene& scene)
	{
		// Search for the root bone
		scene.bone_root = FindFirstNodeContains("_root_", scene.ai_scene->mRootNode);
		assert(scene.bone_root != scene.ai_scene->mRootNode);
		assert(scene.bone_root != nullptr);

		// Compile the pivotless bone tree
		struct BoneTreeNode
		{
			aiNode* node = nullptr;
			int parent = -1;
			mat4 compiled_transform;
		};

	
		std::deque<BoneTreeNode> bone_queue;
		vector<anim::Bone>& skinless_skeleton = scene.skinless_skeleton;
		auto& skinless_skeleton_table = scene.skinless_skeleton_table;

		// Optionally check for pivots here
		bone_queue.push_back(BoneTreeNode{ scene.bone_root, -1 });
		while (!bone_queue.empty())
		{
			auto curr_node = bone_queue.front();
			bone_queue.pop_front();
			string node_name = curr_node.node->mName.data;

			const mat4 node_transform = curr_node.compiled_transform * to_mat4(curr_node.node->mTransformation);

			// Ignore all $assimp$ nodes in the final skeleton. We also ignore user specified excluded nodes. Everything else we will consider it a bone.
			if (node_name.find(AssimpPrefix) != string::npos || node_name.find(AssimpImporter::bone_exclude_keyword) != string::npos)
			{
				for (size_t i = 0; i < curr_node.node->mNumChildren; ++i)
				{
					bone_queue.push_front(BoneTreeNode{ curr_node.node->mChildren[i], curr_node.parent, node_transform });
				}

				continue;
			}

			anim::Bone new_bone;
			new_bone._name = curr_node.node->mName.data;
			new_bone._parent = curr_node.parent;

			// World
			aiMatrix4x4		ai_child_world_bind_pose;// = initMat4(curr_node.assimp_node->_global_inverse_bind_pose).Inverse();
			aiVector3D		child_world_pos;
			aiVector3D		child_world_scale;
			aiQuaternion	child_world_rot;

			// Local
			aiMatrix4x4		ai_child_local_bind_pose;// = b._parent >= 0 ? initMat4(bones_out[b._parent]._global_inverse_bind_pose) * ai_child_world_bind_pose : ai_child_world_bind_pose;
			aiVector3D		child_local_pos;
			aiVector3D		child_local_scale;
			aiQuaternion	child_local_rot;

			auto bone_node = scene.bone_table.find(node_name);
			aiMatrix4x4 global_inverse = curr_node.node->mTransformation;
			global_inverse.Inverse();

			// Multiply the parent's inverse if its there
			if (new_bone._parent >= 0)
			{
				// My global inverse is P(inv) * C_local(inv)
				global_inverse = to_aiMat4(skinless_skeleton[new_bone._parent]._global_inverse_bind_pose) * global_inverse;
			}

			// Initialize child local/global pose
			ai_child_local_bind_pose = to_aiMat4(node_transform);
			ai_child_world_bind_pose = global_inverse;
			ai_child_world_bind_pose.Inverse();

			// Initialize global inverse in bone
			new_bone._global_inverse_bind_pose = to_mat4(global_inverse);

			// World
			aiDecomposeMatrix(&ai_child_world_bind_pose, &child_world_scale, &child_world_rot, &child_world_pos);

			// local
			aiDecomposeMatrix(&ai_child_local_bind_pose, &child_local_scale, &child_local_rot, &child_local_pos);

			// Setting local bind pose of both the new bone and the aiNode
			new_bone._local_bind_pose.position = to_vec3(child_local_pos);
			new_bone._local_bind_pose.rotation = to_quat(child_local_rot);
			new_bone._local_bind_pose.scale = to_vec3(child_local_scale);

			curr_node.node->mTransformation = ai_child_local_bind_pose;

			skinless_skeleton.emplace_back(new_bone);
			skinless_skeleton_table.emplace(skinless_skeleton.back()._name, skinless_skeleton.size() - 1);

			for (size_t i = 0; i < curr_node.node->mNumChildren; ++i)
			{
				bone_queue.push_back(BoneTreeNode{ curr_node.node->mChildren[i], static_cast<int>(skinless_skeleton.size() - 1), mat4{} });
			}
		}
	}
#pragma endregion

#pragma region Compiling/Building Animations
	void CompileAnimations(Scene& scene)
	{
		// Generate anim map.
		// Compile all animated channels.
		// Error checking also happens here.

		// We create a skinless skeleton if the scene has animations but no mesh
		if (scene.ai_scene->HasAnimations() && !scene.ai_scene->HasMeshes())
			BuildSkinlessSkeleton(scene);

		for (size_t i = 0; i < scene.ai_scene->mNumAnimations; ++i)
		{
			aiAnimation* ai_anim = scene.ai_scene->mAnimations[i];
			CompiledAnimation compiled_clip;

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
						PrintError(error_string);
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
					CompileTranslateChannel(scene, animated_bone, ai_channel);
					break;
				case Rotate:
					CompileRotateChannel(scene, animated_bone, ai_channel);
					break;
				case Scale:
					CompileScaleChannel(scene, animated_bone, ai_channel);
					break;
				case Bone:
					CompileBoneChannel(scene, animated_bone, ai_channel);
					break;
				case None:
					assert(false);
					break;
				default:
					assert(false);
					break;
				}

			}

			scene.ai_anim_clips.emplace_back(ai_anim);
			scene.compiled_clips.emplace_back(compiled_clip);
			
			scene.has_animation = true;
		}
	}

	void CompileTranslateChannel(Scene& scene, anim::AnimatedBone& anim_bone, aiNodeAnim* anim_channel)
	{
		UNREFERENCED_PARAMETER(scene);
		// Error handling: There should not be any keys in the scale or rotate section
		if (anim_channel->mNumRotationKeys > 1)
			PrintError(string{ "[Warning]" } + std::to_string(anim_channel->mNumRotationKeys) + 
				" Rotation keys detected inside a translation channel(" + anim_channel->mNodeName.data + "). Ignoring these keys.");
		if (anim_channel->mNumScalingKeys > 1)
			PrintError(string{ "[Warning]" } + std::to_string(anim_channel->mNumScalingKeys) +
				" Scaling keys detected inside a translation channel(" + anim_channel->mNodeName.data + "). Ignoring these keys.");
		if (!anim_bone.translate_track.empty())
		{
			PrintError(string{ "[Warning]" } +
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
			anim_bone.translate_track.emplace_back(to_vec3(pos_key.mValue), s_cast<float>(pos_key.mTime));
		}
	}

	void CompileRotateChannel(Scene& scene, anim::AnimatedBone& anim_bone, aiNodeAnim* anim_channel)
	{
		// Error handling: There should not be any keys in the position or scale section
		if (anim_channel->mNumPositionKeys > 1)
			PrintError(string{ "[Warning]" } +std::to_string(anim_channel->mNumPositionKeys) +
				" Position keys detected inside a translation channel(" + anim_channel->mNodeName.data + "). Ignoring these keys.");
		if (anim_channel->mNumScalingKeys > 1)
			PrintError(string{ "[Warning]" } +std::to_string(anim_channel->mNumScalingKeys) +
				" Scaling keys detected inside a translation channel(" + anim_channel->mNodeName.data + "). Ignoring these keys.");
		if (!anim_bone.rotation_track.empty())
		{
			PrintError(string{ "[Warning]" } +
				"Animated bone " + anim_bone.bone_name + " has more than 1 channel that has rotation keys. Ignoring the one with fewer keys.");

			// Don't put in the keys if the current track has more keys that this channel
			if (anim_bone.rotation_track.size() > anim_channel->mNumRotationKeys)
				return;
		}

		aiNode* node = scene.bone_root->FindNode(anim_channel->mNodeName.data);
		
		// Need to find all the chained rotations first
		aiMatrix4x4 pre_rotation;
		aiIdentityMatrix4(&pre_rotation);
		aiMatrix4x4 post_rotation;
		aiIdentityMatrix4(&post_rotation);

		// Rotation Order: OFFSET_ROT * PIVOT_ROT *  PRE_ROT * !!ROTATION(KEYFRAME)!! * POST_ROT * PIVOT_INV_ROT 

		// Find all the pre transformations. 
		// From the base node, traverse up and check for the pre rotations
		{
			aiNode* parent = node->mParent;
			for (int i = 0; i < PreRotationMax; ++i)
			{
				if (parent->mName.data == anim_bone.bone_name + AssimpPrefix + PreRotateSuffix[i])
				{
					pre_rotation = pre_rotation * parent->mTransformation;
					parent = parent->mParent;
				}
			}
		}
		// Find all post transforms.
		{
			aiNode* child = node->mChildren[0];
			for (int i = 0; i < PostRotationMax; ++i)
			{
				if (child->mName.data == anim_bone.bone_name + AssimpPrefix + PostRotateSuffix[i])
				{
					post_rotation = post_rotation * child->mTransformation;
					child = child->mChildren[0];
				}
			}
		}

		// Pre rotate decompose
		aiVector3D pre_pos;
		aiQuaternion pre_rot;
		aiVector3D pre_scale;
		aiDecomposeMatrix(&pre_rotation, &pre_scale, &pre_rot, &pre_pos);

		// Post rotate decompose
		aiVector3D post_pos;
		aiQuaternion post_rot;
		aiVector3D post_scale;
		aiDecomposeMatrix(&post_rotation, &post_scale, &post_rot, &post_pos);

		quat pre_quat = to_quat(pre_rot);
		quat post_quat = to_quat(post_rot);

		// Error handling : Scale and pos should not be affected.
		if (!pre_scale.Equal(aiVector3D{ 1,1,1 }) || !post_scale.Equal(aiVector3D{ 1,1,1 }))
		{
			PrintError("[Warning] Pre rotation has scale for " + anim_bone.bone_name);
		}

		if (!pre_pos.Equal(aiVector3D{}) || !post_pos.Equal(aiVector3D{ }))
		{
			PrintError("[Warning] Post rotation has translation for " + anim_bone.bone_name);
		}

		for (size_t i = 0; i < anim_channel->mNumRotationKeys; ++i)
		{
			aiQuatKey& rot_key = anim_channel->mRotationKeys[i];
			quat rot_val = to_quat(rot_key.mValue);

			rot_val = pre_quat * rot_val * post_quat;
			anim_bone.rotation_track.emplace_back(rot_val, s_cast<float>(rot_key.mTime));
		}
	}

	void CompileScaleChannel(Scene& scene, anim::AnimatedBone& anim_bone, aiNodeAnim* anim_channel)
	{
		// Error handling: There should not be any keys in the position or scale section
		if (anim_channel->mNumPositionKeys > 1)
			PrintError(string{ "[Warning]" } + std::to_string(anim_channel->mNumPositionKeys) +
				" Position keys detected inside a translation channel(" + anim_channel->mNodeName.data + "). Ignoring these keys.");
		if (anim_channel->mNumRotationKeys > 1)
			PrintError(string{ "[Warning]" } + std::to_string(anim_channel->mNumRotationKeys) +
				" Rotation keys detected inside a translation channel(" + anim_channel->mNodeName.data + "). Ignoring these keys.");
		if (!anim_bone.scale_track.empty())
		{
			PrintError(string{ "[Warning]" } +
				"Animated bone " + anim_bone.bone_name + " has more than 1 channel that has scaling keys. Ignoring the one with fewer keys.");

			// Don't put in the keys if the current track has more keys that this channel
			if (anim_bone.scale_track.size() > anim_channel->mNumScalingKeys)
				return;
		}

		aiNode* node = scene.bone_root->FindNode(anim_channel->mNodeName.data);

		// Need to find all the chained rotations first
		aiMatrix4x4 pre_scale;
		aiIdentityMatrix4(&pre_scale);
		
		// Scaling Order: OFFSET_SCALE * PIVOT_SCALE *  !!SCALE(KEYFRAME)!!

		// Find all the pre transformations. 
		// From the base node, traverse up and check for the pre rotations
		aiNode* parent = node->mParent;
		for (int i = 0; i < PreScaleMax; ++i)
		{
			if (parent->mName.data == anim_bone.bone_name + AssimpPrefix + PreScaleSuffix[i])
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
				PrintError("[Warning] Result of scale key with pre scale has rotation factors.");
			}

			if (!pos.Equal(aiVector3D{}))
			{
				PrintError("[Warning] Result of scale key with pre/post rotation has translation factors.");
			}

			anim_bone.scale_track.emplace_back(to_vec3(scale), s_cast<float>(scale_key.mTime));
		}
	}

	void CompileBoneChannel(Scene& scene, anim::AnimatedBone& anim_bone, aiNodeAnim* anim_channel)
	{
		UNREFERENCED_PARAMETER(scene);
		if (!anim_bone.scale_track.empty())
		{
			PrintError(string{ "[Warning]" } +
				"Animated bone " + anim_bone.bone_name + " has more than 1 channel that has scaling keys. Ignoring the one with fewer keys.");

			// Don't put in the keys if the current track has more keys that this channel
			if (anim_bone.scale_track.size() > anim_channel->mNumScalingKeys)
				return;
		}

		if (!anim_bone.rotation_track.empty())
		{
			PrintError(string{ "[Warning]" } +
				"Animated bone " + anim_bone.bone_name + " has more than 1 channel that has rotation keys. Ignoring the one with fewer keys.");

			// Don't put in the keys if the current track has more keys that this channel
			if (anim_bone.rotation_track.size() > anim_channel->mNumRotationKeys)
				return;
		}

		if (!anim_bone.translate_track.empty())
		{
			PrintError(string{ "[Warning]" } +
				"Animated bone " + anim_bone.bone_name + " has more than 1 channel that has translation keys. Ignoring the one with fewer keys.");

			// Don't put in the keys if the current track has more keys that this channel
			if (anim_bone.translate_track.size() > anim_channel->mNumPositionKeys)
				return;
		}

		// For bone, we just put all the keys in if we can.
		for (size_t i = 0; i < anim_channel->mNumPositionKeys; ++i)
		{
			auto& pos_key = anim_channel->mPositionKeys[i];
			anim_bone.translate_track.emplace_back(to_vec3(pos_key.mValue), s_cast<float>(pos_key.mTime));
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

	void BuildAnimations(Scene& scene)
	{
		UNREFERENCED_PARAMETER(scene);
		// Do all the keyframe optimizations here.
	}
#pragma endregion

#pragma region Building Meshes
	void WriteToVertices(Scene& scene, const  aiMesh* ai_mesh, vector<ai_helpers::Vertex>& vertices, vector<unsigned>& indices)
	{
		// Clear the buffers
		vertices.clear();
		indices.clear();

		// Allocate space for the mesh data
		vertices.reserve(s_cast<size_t>(ai_mesh->mNumVertices));
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

			vertices.emplace_back(Vertex{ vec3{ pos.x, pos.y, pos.z }
										 ,vec3{ normal.x, normal.y, normal.z }
										 ,vec2{ text.x, text.y }
										 ,vec3{ tangent.x, tangent.y, tangent.z }
										 ,vec3{ bi_tangent.x, bi_tangent.y, bi_tangent.z } });

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
			auto res = scene.final_skeleton_table.find(ai_bone->mName.data);
			assert(res != scene.final_skeleton_table.end());

			const int bone_index = static_cast<int>(res->second);
			for (size_t j = 0; j < ai_bone->mNumWeights; ++j)
			{
				float weight = ai_bone->mWeights[j].mWeight;

				unsigned vert_id = ai_bone->mWeights[j].mVertexId;
				vertices[vert_id].AddBoneData(bone_index, weight);
			}
		}
	}
	void BuildMeshOpenGL(Scene& scene, const vector<ai_helpers::Vertex>& vertices, vector<unsigned>& indices, RscHandle<ogl::OpenGLMesh>& mesh_handle)
	{
		UNREFERENCED_PARAMETER(scene);
		auto& mesh = *mesh_handle;
		vector<ogl::OpenGLDescriptor> descriptor
		{
			ogl::OpenGLDescriptor{vtx::Attrib::Position,		sizeof(Vertex), offsetof(Vertex, pos) },
			ogl::OpenGLDescriptor{vtx::Attrib::Normal,			sizeof(Vertex), offsetof(Vertex, normal) },
			ogl::OpenGLDescriptor{vtx::Attrib::UV,				sizeof(Vertex), offsetof(Vertex, uv) },
			ogl::OpenGLDescriptor{vtx::Attrib::Tangent,			sizeof(Vertex), offsetof(Vertex, tangent) },
			ogl::OpenGLDescriptor{vtx::Attrib::Bitangent,		sizeof(Vertex), offsetof(Vertex, bi_tangent) },
			ogl::OpenGLDescriptor{vtx::Attrib::BoneID,			sizeof(Vertex), offsetof(Vertex, bone_ids) },
			ogl::OpenGLDescriptor{vtx::Attrib::BoneWeight,		sizeof(Vertex), offsetof(Vertex, bone_weights) }
		};

		mesh.AddBuffer(
			ogl::OpenGLBuffer{ GL_ARRAY_BUFFER, descriptor }
			.Bind()
			.Buffer(vertices.data(), sizeof(Vertex), s_cast<GLsizei>(vertices.size()))
		);

		mesh.AddBuffer(
			ogl::OpenGLBuffer{ GL_ELEMENT_ARRAY_BUFFER, {} }
			.Bind()
			.Buffer(indices.data(), sizeof(int), s_cast<GLsizei>(indices.size()))
		);
	}

	void WriteToBuffers(Scene& scene, const aiMesh* ai_mesh,
		vector<vec3>& positions, vector<vec3>& normals, vector<vec2>& uvs, vector<vec3>& tangents, vector<vec3>& bi_tangents, vector<ivec4>& bone_ids, vector<vec4>& bone_weights, vector<unsigned>& indices)
	{
		// Clear the buffers
		positions.clear();
		normals.clear();
		uvs.clear();
		tangents.clear();
		bi_tangents.clear();
		bone_ids.clear();
		bone_weights.clear();
		indices.clear();

		// Allocating space in the buffers
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
			auto res = scene.final_skeleton_table.find(ai_bone->mName.data);
			assert(res != scene.final_skeleton_table.end());

			const int bone_index = static_cast<int>(res->second);
			for (size_t j = 0; j < ai_bone->mNumWeights; ++j)
			{
				const float weight = ai_bone->mWeights[j].mWeight;
				const unsigned vert_id = ai_bone->mWeights[j].mVertexId;
				ai_helpers::AddBoneData(bone_index, weight, bone_ids[vert_id], bone_weights[vert_id]);
			}
		}
	}
	void BuildMeshVulknan(Scene& scene, MeshModder& mesh_modder, RscHandle<vkn::VulkanMesh>& mesh_handle,
		vector<vec3>& positions, vector<vec3>& normals, vector<vec2>& uvs, vector<vec3>& tangents, vector<vec3>& bi_tangents, vector<ivec4>& bone_ids, vector<vec4>& bone_weights, vector<unsigned>& indices)
	{
		UNREFERENCED_PARAMETER(scene);
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
	}

#pragma endregion

	// Utility functions
	void PrintError(string_view error)
	{
		std::cout << error << std::endl;
	}

	aiNode* FindFirstNodeContains(string name, aiNode* node)
	{
		if (string{ node->mName.data }.find(name) != string::npos)
			return node;

		// recurse
		for (size_t i = 0; i < node->mNumChildren; ++i)
		{
			auto found = FindFirstNodeContains(name, node->mChildren[i]);
			if(found != nullptr)
				return found;
		}

		return nullptr;
	}

	void DumpNodes(aiNode* node)
	{
		UNREFERENCED_PARAMETER(node);
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
			ai_mat.a1,  ai_mat.a2,  ai_mat.a3,	0.0f,
			ai_mat.b1,  ai_mat.b2,  ai_mat.b3,	0.0f,
			ai_mat.c1,  ai_mat.c2,  ai_mat.c3,	0.0f,
			0.0f,		0.0f,		0.0f,		1.0f
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
	bool flt_equal(float a, float b)
	{
		return abs(a - b) < epsilon;
	}
	bool vec3_equal(const vec3& lhs, const vec3& rhs)
	{
		return	flt_equal(lhs[0], rhs[0]) && 
				flt_equal(lhs[1], rhs[1]) &&
				flt_equal(lhs[2], rhs[2]);
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
		return	flt_equal(lhs[0], rhs[0]) && 
				flt_equal(lhs[1], rhs[1]) &&
				flt_equal(lhs[2], rhs[2]) &&
				flt_equal(lhs[3], rhs[3]);
	}
	bool mat4_equal(const mat4& lhs, const mat4& rhs)
	{
		return	vec4_equal(lhs[0], rhs[0]) &&
				vec4_equal(lhs[1], rhs[1]) &&
				vec4_equal(lhs[2], rhs[2]) &&
				vec4_equal(lhs[3], rhs[3]);
	}

#pragma endregion 
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
		assert(abs(1.0f - (weights_out[0] + weights_out[1] + weights_out[2] + weights_out[3])) < epsilon);
	}

	void Vertex::AddBoneData(int id, float weight)
	{
		ai_helpers::AddBoneData(id, weight, bone_ids, bone_weights);
	}
}