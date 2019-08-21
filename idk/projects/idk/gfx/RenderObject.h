#pragma once
#include <idk.h>
#include <res/Resource.h>
#include <gfx/MaterialInstance.h>

namespace idk
{
	class Mesh;

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

	// static_assert(std::is_trivially_destructible_v<RenderObject>, "destroying render object must be super efficient");
}