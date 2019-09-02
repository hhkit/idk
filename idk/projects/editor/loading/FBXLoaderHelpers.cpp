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
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}
	vec3 Helper::initVec3(const aiVector3D& vec)
	{
		return vec3(vec.x, vec.y, vec.z);
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
			bones_table.emplace(b._name, bones_out.size());

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

					int bone_index = res->second;
					float weight = ai_bone->mWeights[j].mWeight;

					unsigned vert_id = entries[i]._base_vertex + ai_bone->mWeights[j].mVertexId;
					vertices[vert_id].addBoneData(bone_index, weight);
				}
			}
		}
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