#pragma once
//#include <idk.h>

#include <gfx/Framebuffer.h>
#include <gfx/Camera.h>

//#include <math/matrix.h>

#include <gfx/AttachmentViewType.h>

namespace idk {
	struct LightData;
	struct LightmapConfig
	{
		size_t layer_count = 1;
		AttachmentViewType view_type = AttachmentViewType::e2D;
		bool isCubeMap = false;
	};
	class Lightmap {

	public:
		Lightmap() = default;
		virtual ~Lightmap() = default;
		void UpdateCascade(const CameraData& camData,LightData& light, float cas_near, float cas_far);
		void UpdatePointMat(const LightData& light);
		RscHandle<FrameBuffer> InitShadowMap(LightmapConfig config = {});
		RscHandle<FrameBuffer> InitShadowMap(const size_t& layers, AttachmentViewType type, const bool& isCubeMap);
		RscHandle<FrameBuffer> GetShadowMap();
		LightmapConfig GetConfig()const;
		bool NeedLightMap();
		void DeleteShadowMap();
		unsigned GetShadowMapSize() const;

		RscHandle<FrameBuffer> light_map{};
		mat4 cascade_projection{};
		vector<mat4> shadow_vp{};
		real far_plane = 1.f;
		vec3 cam_max = {};
	private:
		
		mat4 oldCamView{};
		real near_plane = 0.f;
		
		vec3 center = {};
		real bound_radius = 0.f;
		vec3 old_center = {};
		vec3 old_light_pos = {};

		unsigned cascade_resolution = 1024;
		unsigned texel_size = cascade_resolution;

		LightmapConfig _config;
	};
};