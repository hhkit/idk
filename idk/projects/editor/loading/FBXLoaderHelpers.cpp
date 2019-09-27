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

	void generateNodeGraphRecurse(const aiNode* ai_node, AssimpNode& parent_node, const BoneSet& bone_set)
	{
		AssimpNode curr_node;
		curr_node._name = ai_node->mName.data;
		curr_node._node_transform = initMat4(ai_node->mTransformation);

		// if (   mesh_set.find(MeshData{ curr_node._name }) != mesh_set.end() || mesh_set.find(MeshData{ curr_node._name + "mesh" }) != mesh_set.end()
		// 	)
		// 	curr_node._ai_type |= MESH;

		auto bone_res = bone_set.find(BoneData{ curr_node._name });
		if (bone_res != bone_set.end())
		{
			curr_node._ai_type |= BONE;
			curr_node._bone_offset = bone_res->_offset;
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

	void initBoneHierarchy(const AssimpNode& root_node, hash_table<string, size_t>& bones_table, vector<anim::Skeleton::Bone>& bones_out)
	{
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
			anim::Skeleton::Bone b{};
			b._name = curr_node.assimp_node->_name;
			b._parent = curr_node.parent;
			b._offset = curr_node.assimp_node->_bone_offset;
			b._node_transform = node_transform;

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

	static void initAnimNodesRecurse(const AssimpNode& assimp_node, hash_table<string, const aiNodeAnim*> ai_anim_table, anim::Animation& anim_clip, vector<anim::Animation::Channel>& virtual_channels)
	{
		
		// Is it animated?
		auto ai_anim_node = ai_anim_table.find(assimp_node._name);
		bool is_animated = ai_anim_node != ai_anim_table.end();

		// Initializing the channel. Every node will have a channel regardless whether it is virtual/animated or not.
		anim::Animation::Channel channel;
		channel._name = assimp_node._name;
		
		// Initialize the key frames if this node is animated. Again, we do not care if this is vritual or not.
		if (is_animated)
			initChannel(channel, ai_anim_node->second);
		else if ((assimp_node._ai_type & MESH) != MESH)
			channel._node_transform = assimp_node._node_transform;

		virtual_channels.emplace_back(channel);
		
		// is it a bone? 
		if ((assimp_node._ai_type & BONE) == BONE)
		{	
			// Add all the channels into the ea_node. This ea_node is now full initialized.
			anim::Animation::EasyAnimNode ea_node;
			assert(ea_node._debug_assert == false);

			ea_node._name = assimp_node._name;
			ea_node._channels = virtual_channels;
			ea_node._debug_assert = true;
			anim_clip.AddEasyAnimNode(ea_node);
			virtual_channels.clear();
		}
		
		// If this is a virtual node, we simply recurse until we find a bone. 
		// If no bone is found, we need to clear the virtual channels.
		for (auto& elem : assimp_node._children)
			initAnimNodesRecurse(elem, ai_anim_table, anim_clip, virtual_channels);

		// We should always clear here. If no bone is found, all the virtual channels will be cleared.
		virtual_channels.clear();
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

		vector<anim::Animation::Channel> channels;
		for (auto& elem : root_node._children)
			initAnimNodesRecurse(elem, ai_anim_table, anim_clip, channels);

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