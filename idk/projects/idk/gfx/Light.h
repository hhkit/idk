#pragma once
#include <idk.h>
#include <core/Component.h>
#include <gfx/LightTypes.h>
#include <gfx/Camera.h>
#pragma warning (push)
#pragma warning (disable : 4324)

namespace idk
{
	struct BaseLightData //Should match exactly with the version in the shader
	{
		int   index = 0;                            //4 ->16
		alignas(16) color light_color = color{ 1, 1, 1 };	  //32
		alignas(16) vec3  v_pos = vec3{};			  //48
		alignas(16) vec3  v_dir = vec3{};			  //64
		real  cos_inner = 0;							  //68
		real  cos_outer = 1;							  //72
		real  falloff = 1;							  //76
		real  shadow_bias{ epsilon };						  //80
		real  intensity{ 1.f };							  //84
		int cast_shadow{ true };						  //88 ->96
		alignas(16) mat4  vp {};						  //160
	};
	struct LightData :BaseLightData
	{
		alignas(16) mat4  v {};							  //
		alignas(16) mat4  p{};							  //
		RscHandle<FrameBuffer> light_map;				  //
	};

	class Light
		: public Component<Light>
	{
	public:
		LightVariant light;
		real         shadow_bias   { epsilon };
		bool         casts_shadows { true };

		void InitShadowMap();
		RscHandle<FrameBuffer>& GetLightMap();
		const RscHandle<FrameBuffer>& GetLightMap()const;
		void SetLightMap(const RscHandle<FrameBuffer>& light_map);

		LightData GenerateLightData() const;
		//CameraData GenerateCameraData() const;
	};
}
#pragma warning(pop )
