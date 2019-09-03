#include "pch.h"
#include "FBXLoaderHelpers.h"
#include <deque>
namespace idk::fbx_loader_detail
{

	mat4 Helper::initMat4(const aiMatrix4x4& mat)
	{
		return mat4(
			mat.a1, mat.b1, mat.c1, mat.d1,
			mat.a2, mat.b2, mat.c2, mat.d2,
			mat.a3, mat.b3, mat.c3, mat.d3,
			mat.a4, mat.b4, mat.c4, mat.d4
		);
	}
	mat4 Helper::initMat4(const aiMatrix3x3& mat)
	{
		return mat4(
			mat.a1, mat.b1, mat.c1, 0.0f,
			mat.a2, mat.b2, mat.c2, 0.0f,
			mat.a3, mat.b3, mat.c3, 0.0f,
			0.0f,	0.0f,	0.0f,	1.0f
		);
	}
	vec3 Helper::initVec3(const aiVector3D& vec)
	{
		return vec3(vec.x, vec.y, vec.z);
	}

	inline quat Helper::initQuat(const aiQuaternion& vec)
	{
		return quat(vec.w, vec.x, vec.y, vec.z);
	}


	void Helper::initAssimpNodes(const aiNode* root_node, AssimpNode& node)
	{
		node._name = root_node->mName.data;
		node._transform = initMat4(root_node->mTransformation);

		for (size_t i = 0; i < root_node->mNumChildren; ++i)
		{
			node._children.emplace_back();
			initAssimpNodeRecurse(root_node->mChildren[i], node._children.back());
		}
	}

	void Helper::initOpenGLBuffers(idk::ogl::OpenGLMesh& mesh, const vector<Vertex>& vertices, const vector<unsigned>& indices)
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

	void Helper::initBoneHierarchy(const aiNode* node, hash_set<string> bones_set, hash_table<string, size_t>& bones_table, vector<anim::Skeleton::Bone>& bones_out)
	{
		struct BoneTreeNode
		{
			int parent;
			const aiNode* node;
		};

		std::deque<BoneTreeNode> queue;
		queue.push_front(BoneTreeNode{ -1, node });

		while (!queue.empty())
		{
			auto curr_node = queue.front();
			queue.pop_front();

			// If this node is not actually a bone, we push all its children into the start of the queue with the parent being the current node's parent.
			if (bones_set.find(curr_node.node->mName.data) == bones_set.end())
			{
				for (size_t i = 0; i < curr_node.node->mNumChildren; ++i)
					queue.push_front(BoneTreeNode{ curr_node.parent, curr_node.node->mChildren[i] });

				continue;
			}

			// If curr_node is a bone, we push it into bones_out with parent being curr_node's parent.
			// We also push the bone into the bones_table. This is so we can easily find a bone by name. The index should be bones_out.size().
			// The parent of all these children should be bones_out.size() - 1.
			anim::Skeleton::Bone b{};
			b._name = curr_node.node->mName.data;
			b._parent = curr_node.parent;
			b._offset = initMat4(curr_node.node->mTransformation);

			bones_out.emplace_back(b);
			bones_table.emplace(bones_out.back()._name, bones_out.size() - 1);

			for (size_t i = 0; i < curr_node.node->mNumChildren; ++i)
				queue.push_back(BoneTreeNode{ static_cast<int>(bones_out.size() - 1), curr_node.node->mChildren[i] });
		}
	}

	void Helper::initBoneWeights(const aiScene* ai_scene, const vector<ogl::OpenGLMesh::MeshEntry>& entries, hash_table<string, size_t>& bones_table, vector<Vertex>& vertices)
	{
		for (size_t i = 0; i < ai_scene->mNumMeshes; ++i)
		{
			const aiMesh* ai_mesh = ai_scene->mMeshes[i];
			for (size_t k = 0; k < ai_mesh->mNumBones; ++k)
			{
				const aiBone* ai_bone = ai_mesh->mBones[k];
				for (size_t j = 0; j < ai_bone->mNumWeights; ++j)
				{
					auto res = bones_table.find(ai_bone->mName.data);
					assert(res != bones_table.end());

					int bone_index = static_cast<int>(res->second);
					float weight = ai_bone->mWeights[j].mWeight;

					unsigned vert_id = entries[i]._base_vertex + ai_bone->mWeights[j].mVertexId;
					vertices[vert_id].addBoneData(bone_index, weight);
				}
			}
		}
	}

	void Helper::initAnimMap(const aiAnimation* ai_anim, anim::Animation& anim_clip)
	{
		for (size_t i = 0; i < ai_anim->mNumChannels; ++i)
		{
			const aiNodeAnim* ai_anim_node = ai_anim->mChannels[i];
			anim::Animation::AnimNode anim_node;

			anim_node._name = ai_anim_node->mNodeName.data;

			for (size_t p = 0; p < ai_anim_node->mNumPositionKeys; ++p)
			{
				auto& position_key = ai_anim_node->mPositionKeys[p];

				const vec3 val = initVec3(position_key.mValue);
				const float time = static_cast<float>(position_key.mTime);
				
				anim_node._translate.emplace_back(val, time);
			}

			for (size_t s = 0; s < ai_anim_node->mNumScalingKeys; ++s)
			{
				auto& scale_key = ai_anim_node->mScalingKeys[s];

				const vec3 val = initVec3(scale_key.mValue);
				const float time = static_cast<float>(scale_key.mTime);
				
				anim_node._scale.emplace_back(val, time);
			}

			for (size_t r = 0; r < ai_anim_node->mNumRotationKeys; ++r)
			{
				auto& rotation_key = ai_anim_node->mRotationKeys[r];

				const quat val = initQuat(rotation_key.mValue);
				const float time = static_cast<float>(rotation_key.mTime);
				
				anim_node._rotation.emplace_back(val, time);
			}

			anim_clip.AddAnimNode(anim_node);
		}

		float fps = static_cast<float>(ai_anim->mTicksPerSecond <= 0.0 ? 24.0f : ai_anim->mTicksPerSecond);
		float duration = static_cast<float>(ai_anim->mDuration);
		float num_ticks = duration / fps;

		anim_clip.SetSpeeds(fps, duration, num_ticks);
	}

	void Helper::initAnimNodeTransforms(const aiNode* root_node, anim::Animation& anim_clip)
	{
		mat4 curr_accum;

		// If we can find the AnimNode with the corresponding name, we set its accum to whatever it was before
		auto res = anim_clip.GetAnimNode(root_node->mName.data);
		if(res == nullptr)
			curr_accum = initMat4(root_node->mTransformation);
		else
			res->_accum = curr_accum;

		for (size_t i = 0; i < root_node->mNumChildren; ++i)
			initAnimNodesRecurse(root_node->mChildren[i], anim_clip, curr_accum);
	}

	void Helper::initAssimpNodeRecurse(const aiNode* ai_node, AssimpNode& node)
	{
		node._name = ai_node->mName.data;
		node._transform = initMat4(ai_node->mTransformation);

		for (size_t i = 0; i < ai_node->mNumChildren; ++i)
		{
			node._children.emplace_back();
			initAssimpNodeRecurse(ai_node->mChildren[i], node._children.back());
		}
	}

	void Helper::initAnimNodesRecurse(const aiNode* node, anim::Animation& anim_clip, const mat4& curr_accum)
	{
		mat4 node_accum;
		auto res = anim_clip.GetAnimNode(node->mName.data);
		if (res == nullptr)
			node_accum = curr_accum * initMat4(node->mTransformation);
		else
			res->_accum = curr_accum;

		for (size_t i = 0; i < node->mNumChildren; ++i)
			initAnimNodesRecurse(node->mChildren[i], anim_clip, node_accum);
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