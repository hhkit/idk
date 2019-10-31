#pragma once
#include <idk.h>

#undef min
#undef max
// Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/config.h>
#include <assimp/cimport.h>

// Core includes
#include <gfx/GraphicsSystem.h>
#include <res/MetaBundle.h>

// Vulkan
#include <vkn/VulkanMesh.h>
#include "vkn/BufferHelpers.h"
#include "vkn/VknMeshModder.h"

// Opengl
#include <opengl/system/OpenGLGraphicsSystem.h>
#include <opengl/resource/OpenGLMesh.h>

// Animation
#include <anim/Skeleton.h>
#include <anim/Animation.h>

// Math
#include <math/matrix_transforms.h>

namespace idk::ai_helpers
{
	using attrib_index = vkn::attrib_index;
	using MeshBuffer = vkn::MeshBuffer;
	using MeshModder = vkn::MeshModder;
	using offset_t = size_t;

#pragma region Helper Structs
	struct Vertex
	{
		vec3	pos;
		vec3	normal;
		vec2	uv;
		vec3	tangent;
		vec3	bi_tangent;
		ivec4	bone_ids;
		vec4	bone_weights;

		void AddBoneData(int id, float weight);
	};

	struct CompiledAnimation
	{
		string name;
		aiAnimation* ai_animation;

		hash_table<string, size_t> animated_bone_table;
		vector<anim::AnimatedBone> animated_bones;
		float fps = 0.0f;
		float num_ticks = 0.0f;
		float duration = 0.0f;
	};

	enum ChannelType
	{
		Bone = 0,
		Translate,
		Rotate,
		Scale,

		// Error
		None = -1
	};

	enum PreRotationIndex
	{
		RotationOffset = 0,
		RotationPivot,
		PreRotation,

		PreRotationMax
	};

	enum PostRotationIndex
	{
		PostRotation = 0,
		RotationPivotInverse,

		PostRotationMax
	};

	enum PreScaleIndex
	{
		ScalingOffset = 0,
		ScalingPivot,

		PreScaleMax
	};
	static string AssimpPrefix = "_$AssimpFbx$_";
	static string PreRotateSuffix[PreRotationMax]
	{
		"RotationOffset",
		"RotationPivot",
		"PreRotation"
	};

	static string PostRotateSuffix[PostRotationMax]
	{
		"PostRotation",
		"RotationPivotInverse"
	};

	static string PreScaleSuffix[PreScaleMax]
	{
		"ScalingOffset",
		"ScalingPivot"
	};

#pragma endregion

	struct Scene
	{
		Assimp::Importer importer;
		const aiScene* ai_scene = nullptr;
		
		// =================================================  Meshes
		hash_table<string, aiMesh*> mesh_table;
		vector<aiNode*> mesh_nodes;
		vector<aiMesh*> meshes;

		// =================================================  Bones
		aiNode* bone_root;
		hash_table<string, aiBone*> bone_table;

		hash_table<string, size_t> final_skeleton_table;
		vector<anim::BoneData> final_skeleton;

		hash_table<string, size_t> skinless_skeleton_table;
		vector<anim::BoneData> skinless_skeleton;

		// =================================================  Animation
		vector<aiAnimation*> ai_anim_clips;
		vector<CompiledAnimation> compiled_clips;


		// =================================================  Checks
		bool has_meshes = false;
		bool has_skinned_meshes = false;
		bool has_skeleton = false;
		bool has_animation = false;

		unsigned num_meshes = 0;

		string file_ext{};
	};

	struct OpenGLMeshBuffers
	{
		vector<ai_helpers::Vertex> vertices;
		vector<unsigned> indices;
	};

	struct VulkanMeshBuffers
	{
		vector<vec3	>	positions;
		vector<vec3	>	normals;
		vector<vec2	>	uvs;
		vector<vec3	>	tangents;
		vector<vec3	>	bi_tangents;
		vector<ivec4>	bone_ids;
		vector<vec4	>	bone_weights;
		vector<unsigned> indices;
	};

	bool Import(Scene& scene, PathHandle handle);
	// Fill in mesh_nodes and meshes.
	void CompileMeshes(Scene& scene, aiNode* node);
	void CompileBones(Scene& scene);

	// should only be called after compile skeleton.
	// Uses the data inside bone_table and the bone_root to generate the pivotless skeleton and the normal skeleton
	void BuildSkeleton(Scene& scene);
	void BuildSkinlessSkeleton(Scene& scene);	// Only call this if there is animation but no meshes

	// Should only be called after skeleton is built. If there is no skeleton, all meshes will have no bone weights

	// Opengl mesh building
	OpenGLMeshBuffers WriteToVertices(Scene& scene, const aiMesh* mesh);
	void BuildMeshOpenGL(Scene& scene, const OpenGLMeshBuffers& mesh_buffers, const RscHandle<ogl::OpenGLMesh>& mesh_handle);

	// Vulkan's cancer 
	VulkanMeshBuffers WriteToBuffers(Scene& scene, const aiMesh* mesh);
	void BuildMeshVulknan(Scene& scene, MeshModder& mesh_modder, RscHandle<vkn::VulkanMesh>& mesh_handle, const VulkanMeshBuffers& mesh_buffers);

	// Animation building. First compile all aiAnimations
	void CompileAnimations(Scene& scene);
	void BuildAnimations(Scene& scene);

	void CompileTranslateChannel(Scene& scene, anim::AnimatedBone& anim_bone, aiNodeAnim* anim_channel);
	void CompileRotateChannel(Scene& scene, anim::AnimatedBone& anim_bone, aiNodeAnim* anim_channel);
	void CompileScaleChannel(Scene& scene, anim::AnimatedBone& anim_bone, aiNodeAnim* anim_channel);
	void CompileBoneChannel(Scene& scene, anim::AnimatedBone& anim_bone, aiNodeAnim* anim_channel);

	// Utility functions
	void PrintError(string_view error);
	aiNode* FindFirstNodeContains(string name, aiNode* node);
	void AddBoneData(unsigned id_in, float weight_in, ivec4& ids_out, vec4& weights_out);
	void DumpNodes(aiNode* node);

	// Conversion
	vec3 to_vec3(const aiVector3D& ai_vec);
	quat to_quat(const aiQuaternion& ai_quat);
	mat4 to_mat4(const aiMatrix4x4& ai_mat);
	mat4 to_mat4(const aiMatrix3x3& ai_mat);
	aiMatrix4x4 to_aiMat4(const mat4& mat);

	// Comparisons
	bool flt_equal(float a, float b, float eps = 0.001f);
	bool vec3_equal(const vec3& lhs, const vec3& rhs, float eps = 0.001f);
	bool vec4_equal(const vec4& lhs, const vec4& rhs);
	bool quat_equal(const quat& lhs, const quat& rhs);
	bool mat4_equal(const mat4& lhs, const mat4& rhs);

}