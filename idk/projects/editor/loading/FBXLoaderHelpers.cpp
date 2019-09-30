#include "pch.h"
#include "FBXLoaderHelpers.h"
#include <math/matrix_decomposition.h>
#include <deque>
namespace idk::fbx_loader_detail
{
	// vec3 FBX_SCALE = vec3{ 100.0f, 100.0f, 100.0f };

	mat4 initMat4(const aiMatrix4x4& mat)
	{
		mat4 a(
			mat.a1, mat.a2, mat.a3, mat.a4,
			mat.b1, mat.b2, mat.b3, mat.b4,
			mat.c1, mat.c2, mat.c3, mat.c4,
			mat.d1, mat.d2, mat.d3, mat.d4
		);
		//auto decomp = decompose(a);
		return a;
	}
	mat4 initMat4(const aiMatrix3x3& mat)
	{
		return mat4(
			mat.a1, mat.a2, mat.a3, 0.0f,
			mat.b1, mat.b2, mat.b3, 0.0f,
			mat.c1, mat.c2, mat.c3, 0.0f,
			0.0f,	0.0f,	0.0f,	1.0f
		);
	}
	vec3 initVec3(const aiVector3D& vec)
	{
		return vec3(vec.x, vec.y, vec.z);
	}

	quat initQuat(const aiQuaternion& vec)
	{
		return quat(vec.x, vec.y, vec.z, vec.w);
	}

	void generateNodeGraphRecurse(const aiNode* ai_node, AssimpNode& parent_node, const BoneSet& bone_set)
	{
		AssimpNode curr_node;
		curr_node._name = ai_node->mName.data;
		curr_node._node_transform = initMat4(ai_node->mTransformation);
		//curr_node._local_bind_pose[3] = curr_node._local_bind_pose[3] * vec4{ FBX_SCALE, 1.0f };

		// if (   mesh_set.find(MeshData{ curr_node._name }) != mesh_set.end() || mesh_set.find(MeshData{ curr_node._name + "mesh" }) != mesh_set.end()
		// 	)
		// 	curr_node._ai_type |= MESH;

		auto bone_res = bone_set.find(BoneData{ curr_node._name });
		if (bone_res != bone_set.end())
		{
			curr_node._ai_type |= BONE;
			curr_node._global_inverse_bind_pose = bone_res->_global_inverse_bind_pose;//*;
			// curr_node._global_inverse_bind_pose[3] = curr_node._global_inverse_bind_pose[3] * vec4{   , 1.0f };
			//auto& trans_vec2 = curr_node._global_inverse_bind_pose[0];
		}

		if (curr_node._name.find("$Assimp$") != string::npos)
		{
			curr_node._ai_type |= VIRTUAL;
		}

		for (size_t i = 0; i < ai_node->mNumChildren; ++i)
		{
			generateNodeGraphRecurse(ai_node->mChildren[i], curr_node, bone_set);
		}

		parent_node._children.emplace_back(std::move(curr_node));
	}

	void generateNodeGraph(const aiNode* ai_node, AssimpNode& root_node, const BoneSet& bone_set)
	{
		root_node._name = ai_node->mName.data;
		root_node._node_transform = initMat4(ai_node->mTransformation);
		// root_node._local_bind_pose[3] = root_node._local_bind_pose[3] * vec4{ FBX_SCALE, 1.0f };
		root_node._ai_type = ROOT;

		for (size_t i = 0; i < ai_node->mNumChildren; ++i)
		{
			generateNodeGraphRecurse(ai_node->mChildren[i], root_node, bone_set);
		}
	}

	void normalizeMeshEntries(vector<Vertex>& vertices, const mat4& matrix)
	{
		for (auto& elem : vertices)
			elem.pos = matrix * vec4{ elem.pos, 1.0f };
	}

	void initOpenGLBuffers(idk::ogl::OpenGLMesh& mesh, const vector<Vertex>& vertices, const vector<unsigned>& indices)
	{
		vector<ogl::OpenGLDescriptor> descriptor
		{
			ogl::OpenGLDescriptor{vtx::Attrib::Position,	sizeof(Vertex), offsetof(Vertex, pos) },
			ogl::OpenGLDescriptor{vtx::Attrib::Normal,		sizeof(Vertex), offsetof(Vertex, normal) },
			ogl::OpenGLDescriptor{vtx::Attrib::UV,			sizeof(Vertex), offsetof(Vertex, uv) },
			ogl::OpenGLDescriptor{vtx::Attrib::BoneID,		sizeof(Vertex), offsetof(Vertex, bone_ids) },
			ogl::OpenGLDescriptor{vtx::Attrib::BoneWeight,	sizeof(Vertex), offsetof(Vertex, bone_weights) }
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

	void initBoneHierarchy(const AssimpNode& root_node, hash_table<string, size_t>& bones_table, vector<anim::Bone>& bones_out, const mat4& normalize)
	{
		normalize;
		struct BoneTreeNode
		{
			int parent;
			const AssimpNode* assimp_node;
			mat4 parent_transform;
		};

		std::deque<BoneTreeNode> queue;

		queue.push_front(BoneTreeNode{ -1, &root_node });
		while (!queue.empty())
		{
			auto curr_node = queue.front();
			queue.pop_front();
			mat4 node_transform = curr_node.parent_transform * curr_node.assimp_node->_node_transform;
			
			// If this node is not actually a bone, we push all its children into the start of the queue with the parent being the current node's parent.
			
			if ((curr_node.assimp_node->_ai_type & BONE) != BONE)
			{
				//for (size_t i = 0; i < curr_node.assimp_node->_children.s; ++i)
				for(auto& elem : curr_node.assimp_node->_children)
				{
					queue.push_front(BoneTreeNode{ curr_node.parent, &elem, node_transform });
				}

				continue;
			}

			// If curr_node is a bone, we push it into bones_out with parent being curr_node's parent.
			// We also push the bone into the bones_table. This is so we can easily find a bone by name. The index should be bones_out.size().
			// The parent of all these children should be bones_out.size() - 1.
			anim::Bone b{};
			b._name = curr_node.assimp_node->_name;
			b._parent = curr_node.parent;

			auto decomp = decompose(curr_node.assimp_node->_global_inverse_bind_pose);
			b._global_inverse_bind_pose = mat4{ scale(fbx_loader_detail::FBX_SCALE) } * curr_node.assimp_node->_global_inverse_bind_pose;
			// b._global_inverse_bind_pose[3] = b._global_inverse_bind_pose[3] * vec4{ FBX_SCALE, 1.0f };
			
			auto local_bind = mat4{ scale(fbx_loader_detail::FBX_SCALE) } *node_transform;
			decomp = decompose(local_bind);
			auto decomp2 = decompose(node_transform);
			b._local_bind_pose.position= decomp.position;// *FBX_SCALE;
			b._local_bind_pose.rotation = decomp.rotation;
			b._local_bind_pose.scale = decomp2.scale;

			bones_out.emplace_back(b);
			bones_table.emplace(bones_out.back()._name, bones_out.size() - 1);

			// for (size_t i = 0; i < curr_node.node->mNumChildren; ++i)
			for (auto& elem : curr_node.assimp_node->_children)
			{
				queue.push_back(BoneTreeNode{ static_cast<int>(bones_out.size() - 1), &elem, mat4{} });
			}
		}
	}

	void initBoneWeights(const aiScene* ai_scene, span<ogl::OpenGLMesh::MeshEntry> entries, hash_table<string, size_t>& bones_table, vector<Vertex>& vertices)
	{
		for (size_t i = 0; i < ai_scene->mNumMeshes; ++i)
		{
			const aiMesh* ai_mesh = ai_scene->mMeshes[i];
			for (size_t k = 0; k < ai_mesh->mNumBones; ++k)
			{
				const aiBone* ai_bone = ai_mesh->mBones[k];
				auto res = bones_table.find(ai_bone->mName.data);
				assert(res != bones_table.end());

				int bone_index = static_cast<int>(res->second);
				for (size_t j = 0; j < ai_bone->mNumWeights; ++j)
				{
					float weight = ai_bone->mWeights[j].mWeight;

					unsigned vert_id = entries[i]._base_vertex + ai_bone->mWeights[j].mVertexId;
					vertices[vert_id].addBoneData(bone_index, weight);
				}
			}
		}
	}

	static void initChannel(anim::Channel& channel, const aiNodeAnim* ai_anim_node, const mat4& concat_matrix)
	{
		auto inverse_mat = concat_matrix.inverse();
		auto decomp = decompose(concat_matrix);

		// POSITION
		if (ai_anim_node->mNumPositionKeys > 1)
		{
			vec3 prev;
			for (size_t p = 1; p < ai_anim_node->mNumPositionKeys; ++p)
			{
				auto& position_key = ai_anim_node->mPositionKeys[p];

				const vec3 curr = initVec3(position_key.mValue);
				//if (abs(prev[0] - curr[0]) > epsilon &&
				//	abs(prev[1] - curr[1]) > epsilon &&
				//	abs(prev[2] - curr[2]) > epsilon)
				{
					const float time = static_cast<float>(position_key.mTime - ai_anim_node->mPositionKeys[1].mTime);

					auto scaled_translate = (curr - decomp.position) * FBX_SCALE;
					channel._translate.emplace_back(scaled_translate, time);
					prev = curr;
				}
			}
			channel._is_animated = true;
		}
		
		// SCALE
		if (ai_anim_node->mNumScalingKeys > 1)
		{
			vec3 prev;
			for (size_t s = 1; s < ai_anim_node->mNumScalingKeys; ++s)
			{
				auto& scale_key = ai_anim_node->mScalingKeys[s];

				const vec3 curr = initVec3(scale_key.mValue);
				//if (abs(prev[0] - curr[0]) > epsilon &&
				//	abs(prev[1] - curr[1]) > epsilon &&
				//	abs(prev[2] - curr[2]) > epsilon)
				{
					const float time = static_cast<float>(scale_key.mTime - ai_anim_node->mScalingKeys[1].mTime);
					auto final_scale = curr - decomp.scale;
					
					for (auto& elem : final_scale)
					{
						if (abs(elem) < epsilon)
							elem = 0.0f;
					}
					channel._scale.emplace_back(final_scale, time);
					prev = curr;
				}
			}
			channel._is_animated = true;
		}

		// ROTATE
		if (ai_anim_node->mNumRotationKeys > 1)
		{
			//quat prev
			for (size_t r = 1; r < ai_anim_node->mNumRotationKeys; ++r)
			{
				auto& rotation_key = ai_anim_node->mRotationKeys[r];
				const quat curr = initQuat(rotation_key.mValue);
				const float time = static_cast<float>(rotation_key.mTime - ai_anim_node->mRotationKeys[1].mTime);

				auto final_rot = curr;// *quat_cast<mat3>(decomp.rotation).inverse();
				//auto decomp_rot = decompose_rotation_matrix(final_rot);
				// for (auto& elem : final_rot)
				// {
				// 	if (abs(elem) < epsilon)
				// 		elem = 0.0f;
				// }
				channel._rotation.emplace_back(final_rot, time);
			}
			channel._is_animated = true;
		}
		
		
	}

	static void initAnimNodesRecurse(const AssimpNode& assimp_node, hash_table<string, const aiNodeAnim*> ai_anim_table, anim::Animation& anim_clip, vector<anim::Channel>& virtual_channels, mat4& concat_transform)
	{
		
		// Is it animated?
		auto ai_anim_node = ai_anim_table.find(assimp_node._name);
		bool is_animated = ai_anim_node != ai_anim_table.end();
		int i;
		// Initializing the channel. Every node will have a channel regardless whether it is virtual/animated or not.
		anim::Channel channel;
		channel._name = assimp_node._name;
		if (channel._name.find("mixamorig:RightHandPinky4") != string::npos)
			i = 5;
		concat_transform = concat_transform * assimp_node._node_transform;
		// Initialize the key frames if this node is animated. Again, we do not care if this is vritual or not.
		if (is_animated)
		{
			initChannel(channel, ai_anim_node->second, concat_transform);
			
			concat_transform = mat4{};
			if(channel._is_animated)
				virtual_channels.emplace_back(channel);
		}
		
		// else if ((assimp_node._ai_type & BONE) == BONE)
		// {
		// 	virtual_channels.emplace_back(channel);
		// }
		//else if ((assimp_node._ai_type & MESH) != MESH)
	    // channel._node_transform = assimp_node._node_transform;
		
		// is it a bone? 
		if ((assimp_node._ai_type & BONE) == BONE)
		{	
			// Add all the channels into the ea_node. This ea_node is now full initialized.
			anim::EasyAnimNode ea_node;
			assert(ea_node._debug_assert == false);

			ea_node._name = assimp_node._name;
			ea_node._channels = virtual_channels;
			ea_node._debug_assert = true;
			anim_clip.AddEasyAnimNode(ea_node);
			virtual_channels.clear();
			concat_transform = mat4{};
		}
		
		// If this is a virtual node, we simply recurse until we find a bone. 
		// If no bone is found, we need to clear the virtual channels.
		for (auto& elem : assimp_node._children)
			initAnimNodesRecurse(elem, ai_anim_table, anim_clip, virtual_channels, concat_transform);

		// We should always clear here. If no bone is found, all the virtual channels will be cleared.
		virtual_channels.clear();
		concat_transform = mat4{};
	}

	void initAnimNodes(const AssimpNode& root_node, const aiAnimation* ai_anim, anim::Animation& anim_clip)
	{
		// Init the anim_node table
		hash_table<string, const aiNodeAnim*> ai_anim_table;
		for (size_t i = 0; i < ai_anim->mNumChannels; ++i)
		{
			const aiNodeAnim* ai_anim_node = ai_anim->mChannels[i];
			ai_anim_table.emplace(ai_anim_node->mNodeName.data, ai_anim_node);
		}

		mat4 concat_transform;
		vector<anim::Channel> channels;
		for (auto& elem : root_node._children)
			initAnimNodesRecurse(elem, ai_anim_table, anim_clip, channels, concat_transform);

		float fps = static_cast<float>(ai_anim->mTicksPerSecond <= 0.0 ? 24.0f : ai_anim->mTicksPerSecond);
		float num_ticks = static_cast<float>(ai_anim->mDuration);
		float duration = num_ticks / fps;

		anim_clip.SetSpeeds(fps, duration, num_ticks);
		anim_clip.SetName(ai_anim->mName.data);
	}

	void Vertex::addBoneData(int id, float weight)
	{
		for (unsigned i = 0; i < 4; i++)
		{
			if (bone_weights[i] == 0.0)
			{
				bone_ids[i] = id;
				bone_weights[i] = weight;
				return;
			}
		}

		// assert(false);
	}
}