#pragma once
#include <idk.h>
#include <res/Resource.h>
#include <gfx/MaterialInstance.h>
#include <gfx/vertex_descriptor.h>

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
		
		//binding,attrib
		hash_table<uint32_t, vtx::Attrib> attrib_bindings;
	};

	// static_assert(std::is_trivially_destructible_v<RenderObject>, "destroying render object must be super efficient");
}