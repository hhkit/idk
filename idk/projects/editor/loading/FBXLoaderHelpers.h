#pragma once
#include <idk.h>

#undef min
#undef max
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/config.h>
#include <assimp/cimport.h>

// System includes
#include <gfx/GraphicsSystem.h>
#include <idk_opengl/system/OpenGLGraphicsSystem.h>

// Resource includes
#include <res/MetaBundle.h>
#include <idk_opengl/resource/OpenGLMesh.h>
#include <vkn/VulkanMesh.h>
#include <anim/Skeleton.h>
#include <anim/Animation.h>

// Math
#include <math/matrix_transforms.h>

namespace idk::fbx_loader_detail
{
	// static vec3 FBX_SCALE = vec3{ 1.0f/100, 1.0f / 100, 1.0f / 100 };
	static vec3 FBX_SCALE = vec3{ 1.0f, 1.0f, 1.0f };
	struct Vertex
	{
		vec3	pos;
		vec3	normal;
		vec2	uv;
		vec3	tangent;
		vec3	bi_tangent;
		ivec4	bone_ids;
		vec4	bone_weights;
		
		void addBoneData(int id, float weight);
	};

	template<typename T>
	struct NameHash
	{
		size_t operator()(const T& data) const
		{
			return std::hash<string>{}(data._name);
		}
	};

	template<typename T>
	struct NameEq
	{
		size_t operator()(const T& lhs, const T& rhs)const
		{
			return lhs._name == rhs._name;
		}
	};


	// Helper structures
	struct BoneData
	{
		BoneData() = default;
		BoneData(string_view name, const mat4& transform = mat4{}) :_name{ name }, _global_inverse_bind_pose{ transform } {}
		string _name;
		mat4 _global_inverse_bind_pose;
	};

	struct MeshData
	{
		MeshData() = default;
		MeshData(string_view name) :_name{ name } {}
		string _name;
	};

	enum AI_NODE_TYPE
	{
		ROOT		= 1 << 0,
		MESH		= 1 << 1,
		BONE_ROOT	= 1 << 2,
		BONE		= 1 << 3,
		BONE_PIVOT	= 1 << 4,
		VIRTUAL		= 1 << 5,

		NONE = 0
	};

	struct AssimpNode
	{
		string _name;
		mat4 _node_transform;
		aiMatrix4x4 _assimp_node_transform;

		mat4 _global_inverse_bind_pose;
		
		bool _has_bone_pivot = false;
		quat _bone_pivot;

		AI_NODE_TYPE _ai_type = NONE;
		
		int _parent = -1;
		vector<int> _children;
	};

	using BoneSet = hash_set<BoneData, NameHash<BoneData>, NameEq<BoneData>>;
	using MeshSet = hash_set<MeshData, NameHash<MeshData>, NameEq<MeshData>>;

	// Convert assimp math to ivan's math
	mat4 initMat4(const aiMatrix4x4& mat);
	mat4 initMat4(const aiMatrix3x3& mat);
	vec3 initVec3(const aiVector3D& vec);
	quat initQuat(const aiQuaternion& vec);

	// Parse the whole assimp node graph into our own format
	void generateNodeGraph(const aiNode* ai_root_node, vector<AssimpNode>& assimp_node_vec, const BoneSet& bone_set);

	// Helper function for initializing bone data
	void normalizeMeshEntries(vector<Vertex>& vertices, const mat4& matrix);
	void initOpenGLBuffers(idk::ogl::OpenGLMesh& mesh, const vector<Vertex>& vertices, const vector<unsigned>& indices);

	// Helper functions for bone data
	void initBoneHierarchy(const vector<AssimpNode>& root_node, hash_table<string, size_t>& bones_table, vector<anim::Bone>& bones_out, const mat4& normalize = mat4{});

	// Helper functions for animation nodes
	void initAnimNodes(const vector<AssimpNode>& root_node, const aiAnimation* ai_anim, anim::Animation& anim_clip);

	void addBoneData(unsigned id_in, float weight_in, ivec4& ids_out, vec4& weights_out);
}