#include "pch.h"
#include "FBXLoaderHelpers.h"

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

	void Helper::initBones(const aiMesh* ai_mesh, vector<Vertex>& vertices, unsigned base_vertex, hash_table<string, size_t>& bones_table, vector<anim::Skeleton::Bone>& bones)
	{
		// We have no concept of heirarchy at this point. Hence, we simply load the bones into a map first.
		// Then, when we traverse the aiNodes, we construct the skeleton based on what nodes come in.
		// The reason why we cannot construct the skeleton when we traverse the aiNodes is because aiNodes can be meshes, virtual, or bones.
		for (size_t i = 0; i < ai_mesh->mNumBones; i++)
		{
			auto curr_bone = ai_mesh->mBones[i];
			string curr_bone_name = curr_bone->mName.data;
			size_t curr_bone_index = 0;

			auto find_res = bones_table.find(curr_bone_name);
			if (find_res == bones_table.end())
			{
				anim::Skeleton::Bone b;
				b._name = curr_bone_name;
				b._offset = initMat4(curr_bone->mOffsetMatrix);

				curr_bone_index = bones.size();
				bones_table.emplace(curr_bone_name, curr_bone_index);
				bones.push_back(b);
			}
			else
			{
				curr_bone_index = find_res->second;
			}

			for (size_t j = 0; j < ai_mesh->mBones[i]->mNumWeights; j++)
			{
				unsigned vert_id = base_vertex + ai_mesh->mBones[i]->mWeights[j].mVertexId;
				float weight = ai_mesh->mBones[i]->mWeights[j].mWeight;
				auto& curr_vert = vertices[vert_id];
				curr_vert.addBoneData(s_cast<int>(curr_bone_index), weight);
			}
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

	void Helper::initBoneHierarchy(aiNode* root_node, hash_table<string, size_t>& bones_table, vector<anim::Skeleton::Bone>& bones)
	{
		UNREFERENCED_PARAMETER(root_node);
		UNREFERENCED_PARAMETER(bones_table);
		UNREFERENCED_PARAMETER(bones);
	}
	void Helper::recurseNodes(aiNode* node, hash_table<string, size_t>& bones_table, vector<anim::Skeleton::Bone>& bones, unsigned parent_index, unsigned next_index)
	{
		unsigned curr_index = next_index;
		for (size_t i = 0; i < node->mNumChildren; ++i)
		{
			aiNode* child = node->mChildren[i];
			auto res = bones_table.find(string{ child->mName.data });
			if (res != bones_table.end())
			{
				bones[res->second]._parent = parent_index;
				std::swap(bones[res->second], bones[next_index++]);
			}
		}
		UNREFERENCED_PARAMETER(curr_index);
		/*bool is_curr_node_valid = false;
		auto res = bones_table.find(string{ node->mName.data }) != bones_table.end();
		if(res != bones_table)
		for (size_t i = 0; i < node->mNumChildren; ++i)
		{
			if(is_curr_node_valid)
				recurseNodes(node->mChildren[i], bones_table, bones, )
		}*/
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