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

	struct AssimpNode
	{
		string _name;
		mat4 _transform;

		vector<AssimpNode> _children;
	};

	struct Helper
	{
		static mat4 initMat4(const aiMatrix4x4& mat);
		static mat4 initMat4(const aiMatrix3x3& mat);
		static vec3 initVec3(const aiVector3D& vec);
		static quat initQuat(const aiQuaternion& vec);

		// Helper function for initializing bone data
		static void initAssimpNodes		(const aiNode* root_node, AssimpNode& node);
		static void initOpenGLBuffers	(idk::ogl::OpenGLMesh& mesh, const vector<Vertex>& vertices, const vector<unsigned>& indices);
		static void initBoneHierarchy	(const aiNode* ai_node, hash_set<string> bones_set, hash_table<string, size_t>& bones_table, vector<anim::Skeleton::Bone>& bones_out);
		static void initBoneWeights		(const aiScene* ai_scene, span<ogl::OpenGLMesh::MeshEntry> entries, hash_table<string, size_t>& bones_table, vector<Vertex>& vertices);
		
		static void initAnimMap(const aiAnimation* ai_anim, anim::Animation& anim_clip);
		static void initAnimNodeTransforms(const aiNode* root_node, anim::Animation& anim_clip);
	private:
		static void initAssimpNodeRecurse(const aiNode* ai_node, AssimpNode& node);
		static void initAnimNodesRecurse(const aiNode* node, anim::Animation& anim_clip, const mat4& curr_accum);
	};
}