#pragma once
#include <idk.h>

#include <gfx/Framebuffer.h>
#include <gfx/Camera.h>

#include <math/matrix.h>

#include <gfx/AttachmentViewType.h>

namespace idk {
	struct LightData;
	class Lightmap {

	public:
		Lightmap() = default;
		virtual ~Lightmap() = default;
		void SetCascade(const CameraData& camData,LightData& light, float cas_near, float cas_far);
		void UpdateResolution(const unsigned& res);
		RscHandle<FrameBuffer> InitShadowMap();
		RscHandle<FrameBuffer> InitShadowMap(size_t layers, AttachmentViewType type);
		RscHandle<FrameBuffer> GetShadowMap();
		bool NeedLightMap();
		void DeleteShadowMap();
		unsigned GetShadowMapSize() const;

		RscHandle<FrameBuffer> light_map{};
		mat4 cascade_projection{};
		real far_plane = 1.f;
		vec3 cam_max = {};
	private:
		
		mat4 oldCamView{};
		real near_plane = 0.f;
		
		vec3 center = {};
		real bound_radius = 0.f;
		vec3 old_center = {};

		unsigned cascade_resolution = 1024;
		unsigned texel_size = cascade_resolution;

		size_t _layer_count=1;
		AttachmentViewType _view_type = AttachmentViewType::e2D;
		
	};
};