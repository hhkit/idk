#pragma once
#include <idk.h>

#undef min
#undef max
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <gfx/GraphicsSystem.h>
#include <idk_opengl/resource/OpenGLMesh.h>
#include <vkn/VulkanMesh.h>
#include <anim/Skeleton.h>
#include <anim/Animation.h>
#include <idk_opengl/system/OpenGLGraphicsSystem.h>

namespace idk::fbx_loader_detail
{
	struct Vertex
	{
		vec3	pos;
		vec3	normal;
		vec2	uv;
		ivec4	bone_ids;
		vec4	bone_weights;

		void addBoneData(int id, float weight);
	};

	struct BoneData
	{
		BoneData() = default;
		BoneData(string_view name, const mat4& transform = mat4{}) :_name{ name }, _transform{ transform } {}
		string _name;
		mat4 _transform;
	};

	struct BoneDataHash
	{
		size_t operator()(const BoneData& data) const
		{
			return std::hash<string>{}(data._name);
		}
	};

	struct BoneDataEqual
	{
		size_t operator()(const BoneData& lhs, const BoneData& rhs)const
		{
			return lhs._name == rhs._name;
		}
	};
	using BoneSet = hash_set<BoneData, BoneDataHash, BoneDataEqual>;

	// Convert assimp math to ivan's math
	mat4 initMat4(const aiMatrix4x4& mat);
	mat4 initMat4(const aiMatrix3x3& mat);
	vec3 initVec3(const aiVector3D& vec);
	quat initQuat(const aiQuaternion& vec);

	// Helper function for initializing bone data
	void initOpenGLBuffers(idk::ogl::OpenGLMesh& mesh, const vector<Vertex>& vertices, const vector<unsigned>& indices);
	void initBoneHierarchy(const aiNode* ai_node, const BoneSet& bones_set, hash_table<string, size_t>& bones_table, vector<anim::Skeleton::Bone>& bones_out);
	void initBoneWeights(const aiScene* ai_scene, span<ogl::OpenGLMesh::MeshEntry> entries, hash_table<string, size_t>& bones_table, vector<Vertex>& vertices);

	void initAnimMap(const aiAnimation* ai_anim, anim::Animation& anim_clip);
	// void initAnimNodeTransforms(const aiNode* root_node, anim::Animation& anim_clip);
	// 
	// void initAnimNodesRecurse(const aiNode* node, anim::Animation& anim_clip, const mat4& curr_accum);

}