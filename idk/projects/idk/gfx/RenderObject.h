#pragma once
#include <idk.h>
#include <res/Resource.h>
#include <gfx/MaterialInstance.h>

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
	};

	struct CameraData
	{
		int  culling_flags = 0xFFFFFFFF;
		mat4 view_matrix;
		mat4 projection_matrix;
		RscHandle<RenderTarget> render_target;
		// variant<> clear_data; // -> support no clear, clear_color, skybox 
	};
	// static_assert(std::is_trivially_destructible_v<RenderObject>, "destroying render object must be super efficient");
}