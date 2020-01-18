#pragma once
#include <idk.h>
#include <res/Resource.h>
#include <gfx/MaterialInstance.h>
#include <gfx/vertex_descriptor.h>
#include <gfx/pipeline_config.h>
#include <gfx/CubeMap.h>
#include <math/rect.h>
#include <gfx/FontData.h>
#include <common/LayerMask.h>
#include <gfx/CameraClear.h>

#include <core/Handle.h>//RenderObject::obj_id
#undef near
#undef far

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
		shared_ptr<const pipeline_config> config{};
		LayerMask layer_mask{ 0xFFFFFFFF };
	};
	inline LayerMask layer_to_mask(char mask)
	{
		return LayerMask{ 1ui32 << mask };
	}

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
		hash_table<string, string> uniform_buffers;
		hash_table<string, RscHandle<Texture>> uniform_textures;
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
		LayerMask culling_flags{ 0xFFFFFFFF };
		mat4 view_matrix{};
		mat4 projection_matrix{};
		RscHandle<RenderTarget> render_target{};
		// variant<> clear_data; // -> support no clear, clear_color, skybox 
		//vec4 clear_color{ 0,0,0,1 };
		bool is_shadow = false;
        CameraClear clear_data;
		opt<RscHandle<Mesh>> CubeMapMesh{};
		rect viewport;
		real near = 0.1f;
		real far = 1.f;
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
	struct ParticleRange
	{
		RscHandle<MaterialInstance> material_instance;
		size_t elem_offset;
		size_t num_elems;
	};

    struct FontRenderData
    {
        vector<FontPoint> coords;
        RscHandle<FontAtlas> atlas;
        color color;
        mat4 transform;
    };

	struct FontRange
	{
		size_t elem_offset;
		size_t num_elems;
	};

    struct ImageData
    {
        RscHandle<Texture> texture;
    };
    struct TextData
    {
        vector<FontPoint> coords;
        RscHandle<FontAtlas> atlas;
    };
	struct UIAttriBlock
	{
		vector<vec2> pos;
		vector<vec2> uv;
	};

	struct UITextRange
	{
		size_t elem_offset;
		size_t num_elems;
	};
    struct UIRenderObject
    {
        mat4 transform;
        color color;
        RscHandle<MaterialInstance> material;
        variant<ImageData, TextData> data;
        unsigned depth; // in scene graph
    };

	struct UIRenderObjectWithCanvas
	{
		vector<UIRenderObject>  ui_ro;
		RscHandle<RenderTarget> render_target;
		size_t num_of_text{ 0 };
	};

	struct alignas(32) UIBlockInfo {
		vec4 color{};
		size_t is_font { 0 };
		size_t is_font2{ 0 };
	};

	struct CanvasRenderRange
	{
		size_t inst_font_begin{ 0 }, inst_font_end{ 0 };
	};


}