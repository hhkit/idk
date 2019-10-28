#pragma once
#include <idk.h>
#include <res/Resource.h>
#include <gfx/MaterialInstance.h>
#include <gfx/vertex_descriptor.h>
#include <gfx/pipeline_config.h>
#include <gfx/CubeMap.h>
#include <gfx/Viewport.h>

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
		const renderer_reqs* renderer_req;
		//hash_table<uint32_t, vtx::Attrib> attrib_bindings;
		shared_ptr<pipeline_config> config{};
	};

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
		using ClearData_t =variant<DontClear, vec4, RscHandle<CubeMap>>;
		GenericHandle obj_id{};
		bool is_scene_camera = false;
		int  culling_flags = 0xFFFFFFFF;
		mat4 view_matrix{};
		mat4 projection_matrix{};
		RscHandle<RenderTarget> render_target{};
		bool overlay_debug_draw{};
		// variant<> clear_data; // -> support no clear, clear_color, skybox 
		//vec4 clear_color{ 0,0,0,1 };
		bool is_shadow = false;
		ClearData_t clear_data;
		opt<RscHandle<Mesh>> CubeMapMesh{};
		Viewport viewport;
	};
	// static_assert(std::is_trivially_destructible_v<RenderObject>, "destroying render object must be super efficient");

    struct ParticleRenderData
    {
        vector<vec3> positions;
        vector<rad> rotations;
        vector<float> sizes;
        vector<color> colors;
        RscHandle<MaterialInstance> material_instance;
    };
}