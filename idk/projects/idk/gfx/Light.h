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
	};

	struct CamLightData
	{
		CameraData cam_data;
		vector<Lightmap> cam_lightmaps;
	};
	struct LightData :BaseLightData
	{
		alignas(16) mat4  vp {};						  //160
		alignas(16) mat4  v {};							  //
		alignas(16) mat4  p{};							  //
		//RscHandle<FrameBuffer> light_map;				  //
		//3 depth attachment (tri-cascade), (near, middle, far)
		vector<Lightmap> light_maps;
		//alignas(16) mat4 w {};
		CameraData camDataRef{};
	};

	class Light
		: public Component<Light>
	{
	public:
		bool         enabled       { true };
		LightVariant light;
		real         shadow_bias   { epsilon };
		bool         casts_shadows { true };
		bool         isolate       { false };

		bool is_active_and_enabled() const;

		void InitShadowMap();
		vector<Lightmap>& GetLightMap();
		const vector<Lightmap>& GetLightMap()const;
		void SetLightMap(const vector<Lightmap>& light_map);

		LightData GenerateLightData() const;
		void SetLightData(const LightData& ld);

		color GetColor()const;
		void SetColor(const color& c);

		real GetLightIntensity()const;
		void SetLightIntensity(const real& i);

		bool GetCastShadow()const { return casts_shadows; }
		void SetCastShadow(const bool& i) { casts_shadows = i; }

		real GetShadowBias()const { return shadow_bias; }
		void SetShadowBias(const real& i) { shadow_bias = i; }
		//CameraData GenerateCameraData() const;
	};
}
#pragma warning(pop )
