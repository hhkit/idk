#pragma once
#include <idk.h>
#include <res/Resource.h>

namespace idk
{
	class Mesh;
	class Material;
	class MaterialInstance;

	struct RenderObject
	{
		// resources
		RscHandle<Mesh>             mesh;
		RscHandle<MaterialInstance> material_instance;

		// transform
		vec3 velocity;
		mat4 transform;

		// lighting
		bool cast_shadows    {};
		bool receive_shadows {};

		// culling
		//sphere bounding_volume;
	};

	static_assert(std::is_trivially_destructible_v<RenderObject>, "destroying render object must be super efficient");
}