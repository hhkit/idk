#pragma once
#include <idk.h>

#include <gfx/Framebuffer.h>
#include <gfx/Camera.h>

#include <math/matrix.h>



namespace idk {
	struct LightData;
	class Lightmap {

	public:
		Lightmap() = default;
		void SetCascade(const CameraData& camData,LightData& light, float cas_near, float cas_far);
		void UpdateResolution(const unsigned& res);
		RscHandle<FrameBuffer> InitShadowMap();
		RscHandle<FrameBuffer> GetShadowMap();
		bool NeedLightMap();

		unsigned GetShadowMapSize() const;

		RscHandle<FrameBuffer> light_map{};
		mat4 cascade_projection{};
		real far_plane = 1.f;
	private:
		
		mat4 oldCamView{};
		real near_plane = 0.f;
		
		vec3 center = {};
		real bound_radius = 0.f;
		vec3 old_center = {};

		unsigned cascade_resolution = 1024;
		unsigned texel_size = cascade_resolution;

		
	};
};