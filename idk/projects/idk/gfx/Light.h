#pragma once
#include <idk.h>
#include <core/Component.h>
#include <gfx/LightTypes.h>
#include <gfx/Camera.h>
#pragma warning (push)
#pragma warning (disable : 4324)

namespace idk
{
	struct LightData
	{
		alignas(16) int   index       = 0;
		alignas(16) color light_color = color{1, 1, 1};
		alignas(16) vec3  v_pos       = vec3{};
		alignas(16) vec3  v_dir       = vec3{};
		real  cos_inner = 0;
		real  cos_outer = 1;
		real  falloff     = 1;
		RscHandle<RenderTarget> light_map;
	};

	class Light
		: public Component<Light>
	{
	public:
		LightVariant light;
		real         shadow_bias   { epsilon };
		bool         casts_shadows { true };

		RscHandle<LightMap>& GetLightMap();
		const RscHandle<LightMap>& GetLightMap()const;
		void SetLightMap(const RscHandle<LightMap>& light_map);

		LightData GenerateLightData() const;
		CameraData GenerateCameraData() const;
	};
}
#pragma warning(pop )
