#pragma once
#include <idk.h>

#undef min
#undef max
// std::filesystem
#include <filesystem>
namespace fs = std::filesystem;

// Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/config.h>
#include <assimp/cimport.h>

// Core includes
#include <gfx/GraphicsSystem.h>
#include <res/MetaBundle.h>

// Animation
#include <anim/Skeleton.h>
#include <anim/Animation.h>

// Math
#include <math/matrix_transforms.h>

namespace idk::ai_helpers
{
	using offset_t = size_t;

#pragma region Helper Structs

	struct AnimationData
	{
		string name;
		aiAnimation* ai_animation;

		hash_table<string, size_t> animated_bone_table;
		vector<anim::AnimatedBone> animated_bones;
		float fps = 0.0f;
		float num_ticks = 0.0f;
		float duration = 0.0f;
	};

	struct MeshData
	{
		string name;

		vector<vec3	>	positions;
		vector<vec3	>	normals;
		vector<vec2	>	uvs;
		vector<vec3	>	tangents;
		vector<vec3	>	bi_tangents;
		vector<ivec4>	bone_ids;
		vector<vec4	>	bone_weights;
		vector<unsigned> indices;
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

	static constexpr string_view AssimpPrefix = "_$AssimpFbx$_";
	static constexpr string_view PreRotateSuffix[]
	{
		"RotationOffset",
		"RotationPivot",
		"PreRotation"
	};

	static constexpr string_view PostRotateSuffix[]
	{
		"PostRotation",
		"RotationPivotInverse"
	};

	static constexpr string_view PreScaleSuffix[]
	{
		"ScalingOffset",
		"ScalingPivot"
	};

#pragma endregion

	struct Scene
	{
		// =================================================  Assimp importing
		Assimp::Importer importer;
		const aiScene* ai_scene = nullptr;
		string file_ext{};

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
		vector<AnimationData> compiled_clips;

		// =================================================  Checks
		bool has_meshes = false;
		bool has_skinned_meshes = false;
		bool has_skeleton = false;
		bool has_animation = false;

		unsigned num_meshes = 0;

		// =================================================  Statics
		static constexpr string_view root_bone_keyword = "_root_";
		static constexpr string_view bone_exclude_keyword = "_bn_exclude_";

		bool Import(const fs::path& fs_path);

		void CollectMeshes(aiNode* node);
		vector<MeshData> BuildMeshBuffers() const;
		CompiledMesh CompileMesh(const MeshData& mesh_buffers) const;

		void CollectBones();
		void BuildSkeleton();
		void BuildSkinlessSkeleton();	// Only call this if there is animation but no meshes
		anim::Skeleton CompileSkeleton() const;
		
		// Animation building. First compile all aiAnimations
		vector<AnimationData> CollectAndBuildAnimations();
		void BuildAnimation(AnimationData& anim_data) const;
		vector<anim::Animation> CompileAnimations();
		
		void BuildTranslateChannel	(anim::AnimatedBone& anim_bone, aiNodeAnim* anim_channel);
		void BuildRotateChannel		(anim::AnimatedBone& anim_bone, aiNodeAnim* anim_channel);
		void BuildScaleChannel		(anim::AnimatedBone& anim_bone, aiNodeAnim* anim_channel);
		void BuildBoneChannel		(anim::AnimatedBone& anim_bone, aiNodeAnim* anim_channel);
	};

	// Utility functions
	void LogWarning(const string& error);
	aiNode* FindFirstNodeContains(string name, aiNode* node);
	void AddBoneData(unsigned id_in, float weight_in, ivec4& ids_out, vec4& weights_out);
	void DumpNodes(aiNode* node);

	quat GetPreRotations(const aiNode* node);
	quat GetPostRotations(const aiNode* node);

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