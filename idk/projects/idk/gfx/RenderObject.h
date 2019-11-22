#pragma once
#include <idk.h>
#include <res/Resource.h>
#include <gfx/MaterialInstance.h>
#include <gfx/vertex_descriptor.h>
#include <gfx/pipeline_config.h>
#include <gfx/CubeMap.h>
#include <math/rect.h>
#include <gfx/FontData.h>

namespace idk
{
	class Mesh;
	class RenderTarget;

	struct RenderObject
	{
		//id
		GenericHandle obj_id{};
		// resources
		RscHandle<Mesh>  mesh;
		RscHandle<MaterialInstance> material_instance;
	//	hash_table<string, hash_table<string,UniformInstance>> uniforms;

		// transform
		vec3 velocity;
		mat4 transform;

		// lighting
		bool cast_shadows    {};
		bool receive_shadows {};

		// culling
		//sphere bounding_volume;
		
		//binding,attrib
		const renderer_attributes* renderer_req;
		//hash_table<uint32_t, vtx::Attrib> attrib_bindings;
		shared_ptr<pipeline_config> config{};
	};
	//template<typename InstancedData>
	struct GenericInstancedRenderObjects :RenderObject
	{
		////id
		//GenericHandle obj_id{};
		//// resources
		//RscHandle<Mesh>  mesh{};
		//RscHandle<MaterialInstance> material_instance{};
		////	hash_table<string, hash_table<string,UniformInstance>> uniforms;
		//
		//// lighting
		//bool cast_shadows{};
		//bool receive_shadows{};
		//
		//// culling
		////sphere bounding_volume;
		//
		////binding,attrib
		//const renderer_attributes* renderer_req{};
		////hash_table<uint32_t, vtx::Attrib> attrib_bindings;
		//shared_ptr<pipeline_config> config{};

		// transform
		size_t instanced_index{}, num_instances{};
	};


	struct InstancedData
	{
		//vec3 velocity  {};
		mat4 transforms{};
		mat4 normal_transforms{};
	};
	struct AnimatedInstancedData :InstancedData
	{
		unsigned skeleton_index{};
	};

	using InstRenderObjects        =GenericInstancedRenderObjects;//<InstancedData>;
	using InstAnimatedRenderObjects=GenericInstancedRenderObjects;//<AnimatedInstancedData>;


	struct SkeletonTransforms
	{
		vector<mat4> bones_transforms;
	};

	struct AnimatedRenderObject
		: RenderObject
	{
		unsigned skeleton_index = 0;
	};

	struct CameraData
	{
		GenericHandle obj_id{};
		unsigned  culling_flags = 0xFFFFFFFF;
		mat4 view_matrix{};
		mat4 projection_matrix{};
		RscHandle<RenderTarget> render_target{};
		// variant<> clear_data; // -> support no clear, clear_color, skybox 
		//vec4 clear_color{ 0,0,0,1 };
		bool is_shadow = false;
        CameraClear clear_data;
		opt<RscHandle<Mesh>> CubeMapMesh{};
		rect viewport;
	};
	// static_assert(std::is_trivially_destructible_v<RenderObject>, "destroying render object must be super efficient");

    struct ParticleObj
    {
        vec3 position;
        rad rotation;
        float size;
        color color;
    };
    struct ParticleRenderData
    {
        vector<ParticleObj> particles;
        RscHandle<MaterialInstance> material_instance;
    };

    struct FontRenderData
    {
        vector<FontPoint> coords;
        RscHandle<FontAtlas> atlas;
        color color;
        mat4 transform;
    };

    struct ImageData
    {
        RscHandle<Texture> texture;
    };
    struct TextData
    {
        vector<FontPoint> coords;
        RscHandle<FontAtlas> atlas;
        int n_size;
    };
    struct UIRenderObject
    {
        mat4 transform;
        color color;
        RscHandle<MaterialInstance> material;
        variant<ImageData, TextData> data;
        unsigned depth; // in scene graph
    };

}