#include "pch.h"
//#include "FBXLoaderHelpers.h"
//#include <math/matrix_decomposition.h>
//#include <deque>
//namespace idk::fbx_loader_detail
//{
//	// vec3 FBX_SCALE = vec3{ 100.0f, 100.0f, 100.0f };
//
//	mat4 initMat4(const aiMatrix4x4& mat)
//	{
//		mat4 a(
//			mat.a1, mat.a2, mat.a3, mat.a4,
//			mat.b1, mat.b2, mat.b3, mat.b4,
//			mat.c1, mat.c2, mat.c3, mat.c4,
//			mat.d1, mat.d2, mat.d3, mat.d4
//		);
//		//auto decomp = decompose(a);
//		return a;
//	}
//	mat4 initMat4(const aiMatrix3x3& mat)
//	{
//		return mat4(
//			mat.a1, mat.a2, mat.a3, 0.0f,
//			mat.b1, mat.b2, mat.b3, 0.0f,
//			mat.c1, mat.c2, mat.c3, 0.0f,
//			0.0f,	0.0f,	0.0f,	1.0f
//		);
//	}
//	vec3 initVec3(const aiVector3D& vec) noexcept
//	{
//		return vec3(vec.x, vec.y, vec.z);
//	}
//
//	quat initQuat(const aiQuaternion& vec)
//	{
//		return quat(vec.x, vec.y, vec.z, vec.w);
//	}
//
//	aiMatrix4x4 initMat4(const mat4& mat)
//	{
//		return aiMatrix4x4{
//			mat[0][0], mat[1][0], mat[2][0], mat[3][0],
//			mat[0][1], mat[1][1], mat[2][1], mat[3][1],
//			mat[0][2], mat[1][2], mat[2][2], mat[3][2],
//			mat[0][3], mat[1][3], mat[2][3], mat[3][3]
//		};
//	}
//
//	void generateNodeGraph(const aiNode* ai_root, vector<AssimpNode>& assimp_node_vec, const BoneSet& bone_set)
//	{
//		struct aiNodeData
//		{
//			const aiNode* ai_node = nullptr;
//			int parent = -1;
//		};
//		aiNodeData queue_start{ ai_root, -1 };
//		std::deque<aiNodeData> ai_data_queue { queue_start };
//		
//		bool first_bone = true;
//		int root_index = -1;
//
//		bool found_pivot = false;
//		quat bone_pivot;
//
//		while (!ai_data_queue.empty())
//		{
//			auto ai_data = ai_data_queue.front();
//			ai_data_queue.pop_front();
//
//			AssimpNode assimp_node;
//			assimp_node._name = ai_data.ai_node->mName.data;
//			assimp_node._node_transform = initMat4(ai_data.ai_node->mTransformation);
//			assimp_node._assimp_node_transform = ai_data.ai_node->mTransformation;
//			assimp_node._parent = ai_data.parent;
//
//			auto bone_res = bone_set.find(BoneData{ assimp_node._name });
//			const int curr_index = s_cast<int>(assimp_node_vec.size());
//			if (assimp_node._parent >= 0)
//			{
//				// add my index to the parent node
//				assimp_node_vec[assimp_node._parent]._children.push_back(curr_index);
//			}
//
//
//			// Check if this node is virtual
//			if (assimp_node._name.find("$AssimpFbx$") != string::npos)
//			{
//				assimp_node._ai_type = VIRTUAL;
//				if (assimp_node._name.find("PreRotation") != string::npos)
//				{
//					bone_pivot = decompose(assimp_node._node_transform).rotation;
//					assimp_node._ai_type |= BONE_PIVOT;
//					found_pivot = true;
//				}
//			}
//			else if (bone_res != bone_set.end())
//			{
//				assimp_node._ai_type					= BONE;
//				assimp_node._global_inverse_bind_pose	= bone_res->_global_inverse_bind_pose;
//				assimp_node._bone_pivot					= bone_pivot;
//				assimp_node._has_bone_pivot				= found_pivot;
//				found_pivot = false;
//
//				// Recurse up and find the first non-virtual, non-ROOT node
//				if (first_bone == true)
//				{
//					int parent_index = assimp_node._parent;
//					root_index = curr_index;
//					while (parent_index > 0)
//					{
//						auto& curr_node = assimp_node_vec[parent_index];
//
//						// If the parent bone is not virtual, we consider it a bone as well.
//						if (curr_node._ai_type != VIRTUAL)
//							curr_node._ai_type = BONE;
//
//						root_index = parent_index;
//						parent_index = curr_node._parent;
//					}
//
//					// The node closest to mRootNode is the bone root
//					first_bone = false;
//				}
//			}
//
//			assimp_node_vec.push_back(assimp_node);
//			
//			for (size_t i = 0; i < ai_data.ai_node->mNumChildren; ++i)
//			{
//				aiNodeData tmp{ ai_data.ai_node->mChildren[i], curr_index };
//				ai_data_queue.push_back(tmp);
//			}
//		}
//		if(root_index >= 0)
//			assimp_node_vec[root_index]._ai_type = BONE_ROOT | BONE;
//	}
//
//	void normalizeMeshEntries(vector<Vertex>& vertices, const mat4& matrix)
//	{
//		for (auto& elem : vertices)
//			elem.pos = matrix * vec4{ elem.pos, 1.0f };
//	}
//
//	void initOpenGLBuffers(idk::ogl::OpenGLMesh& mesh, const vector<Vertex>& vertices, const vector<unsigned>& indices)
//	{
//		vector<ogl::OpenGLDescriptor> descriptor
//		{
//			ogl::OpenGLDescriptor{vtx::Attrib::Position,		sizeof(Vertex), offsetof(Vertex, pos) },
//			ogl::OpenGLDescriptor{vtx::Attrib::Normal,			sizeof(Vertex), offsetof(Vertex, normal) },
//			ogl::OpenGLDescriptor{vtx::Attrib::UV,				sizeof(Vertex), offsetof(Vertex, uv) },
//			ogl::OpenGLDescriptor{vtx::Attrib::Tangent,			sizeof(Vertex), offsetof(Vertex, tangent) },
//			ogl::OpenGLDescriptor{vtx::Attrib::Bitangent,		sizeof(Vertex), offsetof(Vertex, bi_tangent) },
//			ogl::OpenGLDescriptor{vtx::Attrib::BoneID,			sizeof(Vertex), offsetof(Vertex, bone_ids) },
//			ogl::OpenGLDescriptor{vtx::Attrib::BoneWeight,		sizeof(Vertex), offsetof(Vertex, bone_weights) }
//		};
//
//		mesh.AddBuffer(
//			ogl::OpenGLBuffer{ GL_ARRAY_BUFFER, descriptor }
//			.Bind()
//			.Buffer(vertices.data(), sizeof(Vertex), s_cast<GLsizei>(vertices.size()))
//		);
//
//		mesh.AddBuffer(
//			ogl::OpenGLBuffer{ GL_ELEMENT_ARRAY_BUFFER, {} }
//			.Bind()
//			.Buffer(indices.data(), sizeof(int), s_cast<GLsizei>(indices.size()))
//		);
//	}
//
//	void initBoneHierarchy(const vector<AssimpNode>& assimp_node_vec, hash_table<string, size_t>& bones_table, vector<anim::Bone>& bones_out, const mat4& normalize)
//	{
//		normalize;
//		struct BoneTreeNode
//		{
//			int parent;
//			const AssimpNode* assimp_node;
//			mat4 parent_transform;
//		};
//
//		std::deque<BoneTreeNode> queue;
//		// Find the bone root first
//
//		// Find the bone root
//		const AssimpNode* root_bone = &assimp_node_vec[0];
//		for (auto& elem : assimp_node_vec)
//		{
//			if (elem._ai_type == BONE_ROOT)
//			{
//				root_bone = &elem;
//				break;
//			}
//		}
//
//		//if (assimp_node_vec[1]._name.find("character_grp") != string::npos) __debugbreak();
//
//		queue.push_front(BoneTreeNode{ -1, root_bone });
//		while (!queue.empty())
//		{
//			auto curr_node = queue.front();
//			queue.pop_front();
//			const mat4 node_transform = curr_node.parent_transform * curr_node.assimp_node->_node_transform;
//			
//			// If this node is not actually a bone, we push all its children into the start of the queue with the parent being the current node's parent.			
//			if ((curr_node.assimp_node->_ai_type & BONE) != BONE)
//			{
//				//for (size_t i = 0; i < curr_node.assimp_node->_children.s; ++i)
//				for(auto& elem : curr_node.assimp_node->_children)
//				{
//					queue.push_front(BoneTreeNode{ curr_node.parent, &assimp_node_vec[elem], node_transform });
//				}
//
//				continue;
//			}
//
//			// If curr_node is a bone, we push it into bones_out with parent being curr_node's parent.
//			// We also push the bone into the bones_table. This is so we can easily find a bone by name. The index should be bones_out.size().
//			// The parent of all these children should be bones_out.size() - 1.
//			anim::BoneData b{};
//			b._name = curr_node.assimp_node->_name;
//			b._parent = curr_node.parent;
//
//			// auto decomp = decompose(curr_node.assimp_node->_global_inverse_bind_pose);
//			b._global_inverse_bind_pose = curr_node.assimp_node->_global_inverse_bind_pose;
//			
//			// World
//			aiMatrix4x4 ai_child_world_bind_pose = initMat4(curr_node.assimp_node->_global_inverse_bind_pose).Inverse();
//			// Local
//			aiMatrix4x4 ai_child_local_bind_pose = b._parent >= 0 ?  initMat4(bones_out[b._parent]._global_inverse_bind_pose) * ai_child_world_bind_pose : ai_child_world_bind_pose;
//
//			aiVector3D child_world_pos;
//			aiVector3D child_world_scale;
//			aiQuaternion child_world_rot;
//
//			aiVector3D		child_local_pos;
//			aiVector3D		child_local_scale;
//			aiQuaternion	child_local_rot;
//
//			// World
//			aiDecomposeMatrix(&ai_child_world_bind_pose, &child_world_scale, &child_world_rot, &child_world_pos);
//			
//			// local
//			aiDecomposeMatrix(&ai_child_local_bind_pose, &child_local_scale, &child_local_rot, &child_local_pos);
//
//			// if (b._name.find("COG_jnt") != string::npos) __debugbreak();
//
//			b._local_bind_pose.position = initVec3(child_local_pos);// decomp2.position;
//			b._local_bind_pose.rotation = initQuat(child_local_rot);// decomp2.rotation;
//			b._local_bind_pose.scale	= initVec3(child_local_scale);// decomp2.scale;
//
//			//auto recomp_decomp2 = b._local_bind_pose.recompose();
//			bones_out.emplace_back(b);
//			bones_table.emplace(bones_out.back()._name, bones_out.size() - 1);
//
//			// for (size_t i = 0; i < curr_node.node->mNumChildren; ++i)
//			for (auto& elem : curr_node.assimp_node->_children)
//			{
//				queue.push_back(BoneTreeNode{ static_cast<int>(bones_out.size() - 1), &assimp_node_vec[elem], mat4{} });
//			}
//		}
//	}
//
//	static void initChannel(anim::AnimatedBone& channel, const aiNodeAnim* ai_anim_node, const mat4& concat_matrix)
//	{
//		//auto inverse_mat = concat_matrix.inverse();
//		//auto decomp = decompose(concat_matrix);
//		//auto decomp2 = decompose(concat_matrix);
//
//		const aiMatrix4x4 assimp_mat = initMat4(concat_matrix);
//		aiVector3D pos;
//		aiVector3D scale;
//		aiQuaternion rot;
//		aiDecomposeMatrix(&assimp_mat, &scale, &rot, &pos);
//		matrix_decomposition<real> decomp;
//		//* 
//		decomp.position = initVec3(pos);
//		decomp.rotation = initQuat(rot);
//		decomp.scale	= initVec3(scale);
//		
//		// POSITION
//		if (ai_anim_node->mNumPositionKeys > 1)
//		{
//			vec3 prev;
//			for (size_t p = 0; p < ai_anim_node->mNumPositionKeys; ++p)
//			{
//				const auto& position_key = ai_anim_node->mPositionKeys[p];
//
//				const vec3 curr = initVec3(position_key.mValue);
//				const float time = static_cast<float>(position_key.mTime - ai_anim_node->mPositionKeys[0].mTime);
//
//				auto scaled_translate = curr + decomp.position;
//				channel._translate.emplace_back(scaled_translate, time);
//				prev = curr;
//			}
//			channel._is_animated = true;
//		}
//		
//		// SCALE
//		if (ai_anim_node->mNumScalingKeys > 1)
//		{
//			vec3 prev;
//			for (size_t s = 0; s < ai_anim_node->mNumScalingKeys; ++s)
//			{
//				const auto& scale_key = ai_anim_node->mScalingKeys[s];
//
//				const vec3 curr = initVec3(scale_key.mValue);
//				const float time = static_cast<float>(scale_key.mTime - ai_anim_node->mScalingKeys[0].mTime);
//				auto final_scale = curr * decomp.scale;
//
//				for (auto& elem : final_scale)
//				{
//					if (abs(elem) < epsilon)
//						elem = 0.0f;
//				}
//				channel._scale.emplace_back(final_scale, time);
//				prev = curr;
//			}
//			channel._is_animated = true;
//		}
//
//		// ROTATE
//		if (ai_anim_node->mNumRotationKeys > 1)
//		{
//			//quat prev
//			for (size_t r = 0; r < ai_anim_node->mNumRotationKeys; ++r)
//			{
//				const auto& rotation_key = ai_anim_node->mRotationKeys[r];
//				const quat curr = initQuat(rotation_key.mValue);
//				const float time = static_cast<float>(rotation_key.mTime - ai_anim_node->mRotationKeys[0].mTime);
//
//				auto final_rot = decomp.rotation * curr;
//				channel._rotation.emplace_back(final_rot, time);
//			}
//			channel._is_animated = true;
//		}
//	}
//
//	static void initAnimNodesRecurse(const vector<AssimpNode>& assimp_node_vec, int index, hash_table<string, const aiNodeAnim*> ai_anim_table, anim::Animation& anim_clip, vector<anim::AnimatedBone>& virtual_channels, mat4& concat_transform)
//	{
//		
//		auto& assimp_node = assimp_node_vec[index];
//		// Is it animated?
//		auto ai_anim_node = ai_anim_table.find(assimp_node._name);
//		bool is_animated = ai_anim_node != ai_anim_table.end();
//		
//		// Initializing the channel. Every node will have a channel regardless whether it is virtual/animated or not.
//		anim::AnimatedBone channel;
//		channel._name = assimp_node._name;
//		
//		// We don't apply the bone pivot because the pivot is always applied regardless of the animation. 
//		// It is also not accounted for in aiNodeAnim's rotation channel
//		//if(!is_animated)
//		//	concat_transform = concat_transform * assimp_node._node_transform;
//		// auto decomp_test1 = decompose(concat_transform);
//		// auto decomp_test2 = decompose(assimp_node._node_transform);
//		// if (assimp_node._name == "Shoulders")
//		// 	__debugbreak();
//		
//		// Initialize the key frames if this node is animated. Again, we do not care if this is vritual or not.
//		if (is_animated)
//		{
//			initChannel(channel, ai_anim_node->second, concat_transform);
//			
//			concat_transform = mat4{};
//			if(channel._is_animated)
//				virtual_channels.emplace_back(channel);
//		}
//		else if (assimp_node._name.find("PostRotation") == string::npos)
//			concat_transform = concat_transform * assimp_node._node_transform;
//		
//		// is it a bone? 
//		if ((assimp_node._ai_type & BONE) == BONE)
//		{	
//			// Add all the channels into the ea_node. This ea_node is now full initialized.
//			anim::EasyAnimNode ea_node;
//			assert(ea_node._debug_assert == false);
//		
//			ea_node._name = assimp_node._name;
//			ea_node._channels = virtual_channels;
//			ea_node._debug_assert = true;
//			anim_clip.AddAnimatedBone(ea_node);
//			virtual_channels.clear();
//			concat_transform = mat4{};
//
//			if (assimp_node._name.find("R_hip_jnt") != string::npos) __debugbreak();
//		}
//		
//		// If this is a virtual node, we simply recurse until we find a bone. 
//		// If no bone is found, we need to clear the virtual channels.
//		for (auto& elem : assimp_node._children)
//			initAnimNodesRecurse(assimp_node_vec, elem, ai_anim_table, anim_clip, virtual_channels, concat_transform);
//		
//		// We should always clear here. If no bone is found, all the virtual channels will be cleared.
//		virtual_channels.clear();
//		concat_transform = mat4{};
//	}
//
//	void initAnimNodes(const vector<AssimpNode>& assimp_node_vec, const aiAnimation* ai_anim, anim::Animation& anim_clip)
//	{
//		// Init the anim_node table
//		hash_table<string, const aiNodeAnim*> ai_anim_table;
//		for (size_t i = 0; i < ai_anim->mNumChannels; ++i)
//		{
//			const aiNodeAnim* ai_anim_node = ai_anim->mChannels[i];
//			ai_anim_table.emplace(ai_anim_node->mNodeName.data, ai_anim_node);
//		}
//
//		mat4 concat_transform;
//		vector<anim::AnimatedBone> channels;
//		auto& root_node = assimp_node_vec[0];
//		for (auto& elem : root_node._children)
//			initAnimNodesRecurse(assimp_node_vec, elem, ai_anim_table, anim_clip, channels, concat_transform);
//
//		const float fps = static_cast<float>(ai_anim->mTicksPerSecond <= 0.0 ? 24.0f : ai_anim->mTicksPerSecond);
//		const float num_ticks = static_cast<float>(ai_anim->mDuration);
//		const float duration = num_ticks / fps;
//
//		anim_clip.SetSpeeds(fps, duration, num_ticks);
//		anim_clip.SetName(ai_anim->mName.data);
//	}
//
//	void addBoneData(unsigned id_in, float weight_in, ivec4& ids_out, vec4& weights_out)
//	{
//		for (unsigned i = 0; i < 4; i++)
//		{
//			if (weights_out[i] == 0.0)
//			{
//				ids_out[i] = id_in;
//				weights_out[i] = weight_in;
//				return;
//			}
//		}
//		// Need to get the bone with the lowest weight and replace it with this one
//		unsigned min_index = 0;
//		for (unsigned i = 1; i < 4; i++)
//		{
//			if (weights_out[i] < weights_out[min_index])
//			{
//				min_index = i;
//			}
//		}
//
//		if (weight_in > weights_out[min_index])
//		{
//			ids_out[min_index] = id_in;
//			weights_out[min_index] = weight_in;
//		}
//
//		// Normalize all weights
//		auto sum_weights = weights_out[0] + weights_out[1] + weights_out[2] + weights_out[3];
//		weights_out /= sum_weights;
//		assert(abs(1.0f - (weights_out[0] + weights_out[1] + weights_out[2] + weights_out[3])) < epsilon);
//	}
//
//	void Vertex::addBoneData(int id, float weight)
//	{
//		for (unsigned i = 0; i < 4; i++)
//		{
//			if (bone_weights[i] == 0.0)
//			{
//				bone_ids[i] = id;
//				bone_weights[i] = weight;
//				return;
//			}
//		}
//		// Need to get the bone with the lowest weight and replace it with this one
//		unsigned min_index = 0;
//		for (unsigned i = 1; i < 4; i++)
//		{
//			if (bone_weights[i] < bone_weights[min_index])
//			{
//				min_index = i;
//			}
//		}
//
//		if (weight > bone_weights[min_index])
//		{
//			bone_ids[min_index] = id;
//			bone_weights[min_index] = weight;
//		}
//
//		// Normalize all weights
//		auto sum_weights = bone_weights[0] + bone_weights[1] + bone_weights[2] + bone_weights[3];
//		bone_weights /= sum_weights;
//		assert(abs(1.0f - (bone_weights[0] + bone_weights[1] + bone_weights[2] + bone_weights[3])) < epsilon);
//	}
//}