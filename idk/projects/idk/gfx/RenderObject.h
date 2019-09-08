#pragma once
#include <idk.h>
#include <res/Resource.h>
#include <gfx/MaterialInstance.h>
#include <gfx/vertex_descriptor.h>
#include <gfx/pipeline_config.h>

namespace idk
{
	class Mesh;
	class RenderTarget;

	struct RenderObject
	{
		// resources
		RscHandle<Mesh>  mesh;
		MaterialInstance material_instance;

		// transform
		vec3 velocity;
		mat4 transform;

		// lighting
		bool cast_shadows    {};
		bool receive_shadows {};

		// culling
		//sphere bounding_volume;
		
		//binding,attrib
		hash_table<uint32_t, vtx::Attrib> attrib_bindings;
		shared_ptr<pipeline_config> config;
	};

	struct CameraData
	{
		int  culling_flags = 0xFFFFFFFF;
		mat4 view_matrix;
		mat4 projection_matrix;
		RscHandle<RenderTarget> render_target;
		// variant<> clear_data; // -> support no clear, clear_color, skybox 
		vec4 clear_color{ 0,0,0,1 };
	};
	// static_assert(std::is_trivially_destructible_v<RenderObject>, "destroying render object must be super efficient");
}