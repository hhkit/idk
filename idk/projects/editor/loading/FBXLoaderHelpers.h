#pragma once
#include <idk.h>

#undef min
#undef max
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <gfx/GraphicsSystem.h>
#include <idk_opengl/resource/OpenGLMesh.h>
#include <anim/Skeleton.h>
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

	struct Helper
	{
		static inline mat4 initMat4(const aiMatrix4x4& mat);
		static inline mat4 initMat4(const aiMatrix3x3& mat);
		static inline vec3 initVec3(const aiVector3D& vec);

		// Helper function for initializing bone data
		static void initBones(const aiMesh* ai_mesh, 
							  vector<Vertex>& vertices, unsigned base_vertex,									// Vertices
							  hash_table<string, size_t>& bones_table, vector<anim::Skeleton::Bone>& bones);	// Bones

		static void initOpenGLBuffers(idk::ogl::OpenGLMesh& mesh, const vector<Vertex>& vertices, const vector<unsigned>& indices);
		static void initBoneHierarchy(aiNode* root_node, hash_table<string, size_t>& bones_table, vector<anim::Skeleton::Bone>& bones);
	private:
		void recurseNodes(aiNode* node, hash_table<string, size_t>& bones_table, vector<anim::Skeleton::Bone>& bones, unsigned parent_index, unsigned next_index);
	};
}