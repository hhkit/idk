#include "pch.h"
#include "FBXLoaderHelpers.h"
#include <math/matrix_decomposition.h>
#include <deque>
namespace idk::fbx_loader_detail
{
	mat4 initMat4(const aiMatrix4x4& mat)
	{
		mat4 a(
			mat.a1, mat.a2, mat.a3, mat.a4,
			mat.b1, mat.b2, mat.b3, mat.b4,
			mat.c1, mat.c2, mat.c3, mat.c4,
			mat.d1, mat.d2, mat.d3, mat.d4
		);
		auto decomp = decompose(a);
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

	void initBoneHierarchy(const aiNode* node, const BoneSet& bones_set, hash_table<string, size_t>& bones_table, vector<anim::Skeleton::Bone>& bones_out)
	{
		struct BoneTreeNode
		{
			int parent;
			const aiNode* node;
			mat4 parent_transform;
		};

		std::deque<BoneTreeNode> queue;

		queue.push_front(BoneTreeNode{ -1, node });
		while (!queue.empty())
		{
			auto curr_node = queue.front();
			queue.pop_front();
			mat4 node_transform = curr_node.parent_transform * initMat4(curr_node.node->mTransformation);
			
			// If this node is not actually a bone, we push all its children into the start of the queue with the parent being the current node's parent.
			auto bone_data = bones_set.find(BoneData{ curr_node.node->mName.data });
			if (bone_data == bones_set.end())
			{
				for (size_t i = 0; i < curr_node.node->mNumChildren; ++i)
				{
					queue.push_front(BoneTreeNode{ curr_node.parent, curr_node.node->mChildren[i], node_transform });
				}

				continue;
			}

			// If curr_node is a bone, we push it into bones_out with parent being curr_node's parent.
			// We also push the bone into the bones_table. This is so we can easily find a bone by name. The index should be bones_out.size().
			// The parent of all these children should be bones_out.size() - 1.
			anim::Skeleton::Bone b{};
			b._name = curr_node.node->mName.data;
			b._parent = curr_node.parent;
			b._offset = bone_data->_transform;
			b._node_transform = node_transform;

			bones_out.emplace_back(b);
			bones_table.emplace(bones_out.back()._name, bones_out.size() - 1);

			for (size_t i = 0; i < curr_node.node->mNumChildren; ++i)
			{
				queue.push_back(BoneTreeNode{ static_cast<int>(bones_out.size() - 1), curr_node.node->mChildren[i], mat4{} });
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

	static void initChannel(anim::Animation::Channel& channel, const aiNodeAnim* ai_anim_node)
	{
		// POSITION
		if (ai_anim_node->mNumPositionKeys <= 2)
		{
			auto& position_key = ai_anim_node->mPositionKeys[0];

			const vec3 val = initVec3(position_key.mValue);
			const float time = static_cast<float>(position_key.mTime);

			channel._translate.emplace_back(val, time);

			if (ai_anim_node->mNumPositionKeys == 2 && position_key != ai_anim_node->mPositionKeys[1])
			{
				const vec3 val_2 = initVec3(ai_anim_node->mPositionKeys[1].mValue);
				const float time_2 = static_cast<float>(ai_anim_node->mPositionKeys[1].mTime);

				channel._translate.emplace_back(val_2, time_2);
			}
		}
		else
		{
			for (size_t p = 0; p < ai_anim_node->mNumPositionKeys; ++p)
			{
				auto& position_key = ai_anim_node->mPositionKeys[p];

				const vec3 val = initVec3(position_key.mValue);
				const float time = static_cast<float>(position_key.mTime);

				channel._translate.emplace_back(val, time);
			}
		}
		
		// SCALE
		if (ai_anim_node->mNumScalingKeys <= 2)
		{
			auto& scale_key = ai_anim_node->mScalingKeys[0];

			const vec3 val = initVec3(scale_key.mValue);
			const float time = static_cast<float>(scale_key.mTime);

			channel._scale.emplace_back(val, time);
			if (ai_anim_node->mNumScalingKeys == 2 && scale_key != ai_anim_node->mScalingKeys[1])
			{
				const vec3 val_2 = initVec3(ai_anim_node->mScalingKeys[1].mValue);
				const float time_2 = static_cast<float>(ai_anim_node->mScalingKeys[1].mTime);

				channel._scale.emplace_back(val_2, time_2);
			}
		}
		else
		{
			for (size_t s = 0; s < ai_anim_node->mNumScalingKeys; ++s)
			{
				auto& scale_key = ai_anim_node->mScalingKeys[s];

				const vec3 val = initVec3(scale_key.mValue);
				const float time = static_cast<float>(scale_key.mTime);

				channel._scale.emplace_back(val, time);
			}
		}
		
		// ROTATE
		if (ai_anim_node->mNumRotationKeys <= 2)
		{
			auto& rotation_key = ai_anim_node->mRotationKeys[0];

			const quat val = initQuat(rotation_key.mValue);
			const float time = static_cast<float>(rotation_key.mTime);

			channel._rotation.emplace_back(val, time);
			if (ai_anim_node->mNumRotationKeys == 2 && rotation_key != ai_anim_node->mRotationKeys[1])
			{
				const quat val_2 = initQuat(ai_anim_node->mRotationKeys[1].mValue);
				const float time_2 = static_cast<float>(ai_anim_node->mRotationKeys[1].mTime);

				channel._rotation.emplace_back(val_2, time_2);
			}
		}
		else
		{
			for (size_t r = 0; r < ai_anim_node->mNumRotationKeys; ++r)
			{
				auto& rotation_key = ai_anim_node->mRotationKeys[r];

				const quat val = initQuat(rotation_key.mValue);
				const float time = static_cast<float>(rotation_key.mTime);

				channel._rotation.emplace_back(val, time);
			}
		}
		
		channel._is_animated = true;
		
	}

	static void initAnimNodesRecurse(const aiNode* ai_node, hash_table<string, const aiNodeAnim*> ai_anim_table, anim::Animation& anim_clip, vector<anim::Animation::Channel>& virtual_channels)
	{
		// Is it animated?
		auto ai_anim_node = ai_anim_table.find(ai_node->mName.data);
		bool is_animated = ai_anim_node != ai_anim_table.end();

		// Initializing the channel. Every node will have a channel regardless whether it is virtual/animated or not.
		anim::Animation::Channel channel;
		channel._name = ai_node->mName.data;
		
		// Initialize the key frames if this node is animated. Again, we do not care if this is vritual or not.
		if (is_animated)
			initChannel(channel, ai_anim_node->second);
		else
		{
			channel._node_transform = initMat4(ai_node->mTransformation);
		}

		virtual_channels.emplace_back(channel);
		
		auto ea_node = anim_clip.GetEasyAnimNode(ai_node->mName.data);
		
		// is it a bone? 
		if (ea_node != nullptr)
		{	
			// Add all the channels into the ea_node. This ea_node is now full initialized.
			assert(ea_node->_debug_assert == false);

			ea_node->_channels = virtual_channels;
			ea_node->_debug_assert = true;
			virtual_channels.clear();
		}
		
		// If this is a virtual node, we simply recurse until we find a bone. 
		// If no bone is found, we need to clear the virtual channels.
		for (size_t i = 0; i < ai_node->mNumChildren; ++i)
			initAnimNodesRecurse(ai_node->mChildren[i], ai_anim_table, anim_clip, virtual_channels);

		// We should always clear here. If no bone is found, all the virtual channels will be cleared.
		virtual_channels.clear();
	}

	void initAnimNodes(const aiNode* ai_root, const aiAnimation* ai_anim, const BoneSet& bones_set, anim::Animation& anim_clip)
	{
		for (auto& elem : bones_set)
		{
			anim::Animation::EasyAnimNode ea_node;
			ea_node._name = elem._name;

			anim_clip.AddEasyAnimNode(ea_node);
		}

		// Init the anim_node table
		hash_table<string, const aiNodeAnim*> ai_anim_table;
		for (size_t i = 0; i < ai_anim->mNumChannels; ++i)
		{
			const aiNodeAnim* ai_anim_node = ai_anim->mChannels[i];
			ai_anim_table.emplace(ai_anim_node->mNodeName.data, ai_anim_node);
		}

		// Is it animated?
		// auto ai_anim_node = ai_anim_table.find(ai_root->mName.data);
		// bool is_animated = ai_anim_node != ai_anim_table.end();
		// 
		// anim::Animation::EasyAnimNode* ea_node = anim_clip.GetEasyAnimNode(ai_root->mName.data);
		// 
		// anim::Animation::Channel channel;
		// channel._node_transform = initMat4(ai_root->mTransformation);
		// ea_node->_channels.emplace_back(channel);
		// 
		// if (is_animated)
		// {
		// 	initChannel(ea_node->_channels.back(), ai_anim_node->second);
		// }

		vector<anim::Animation::Channel> channels;
		for (size_t i = 0; i < ai_root->mNumChildren; ++i)
			initAnimNodesRecurse(ai_root->mChildren[i], ai_anim_table, anim_clip, channels);

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