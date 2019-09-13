#pragma once
#include <idk.h>
#include <core/Component.h>
#include <gfx/LightTypes.h>

namespace idk
{
	struct LightData
	{
		int   index       = 0;
		color light_color = color{1, 1, 1};
		vec3  v_pos       = vec3{};
		vec3  v_dir       = vec3{};
		real  cos_inner   = 0;
		real  cos_outer   = 1;
		real  falloff     = 1;
	};

	class Light
		: public Component<Light>
	{
	public:
		LightVariant light;
		real         shadow_bias   { epsilon };
		bool         casts_shadows { true };

		LightData GenerateLightData() const;
	};
}