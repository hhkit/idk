#include "stdafx.h"

#include <gfx/Lightmap.h>
#ifndef  _LIGHTINCLUDE_H
#define _LIGHTINCLUDE_H
#include <gfx/Light.h>
#endif // ! _LIGHTINCLUDE_H
#include <gfx/RenderTarget.h>
#include <gfx/FramebufferFactory.h>
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>
#include <math/matrix_transforms.inl>
#include <math/matrix_decomposition.inl>
#include <math/quaternion.inl>
#include <gfx/DebugRenderer.h>
#include <math/shapes/frustum.h>
#include <math/shapes/aabb.h>

namespace idk {
	
	//#pragma optimize("",off)
	void Lightmap::SetCascade(const CameraData& camData,LightData& light, float cas_near, float cas_far)
	{
		if (oldCamView != camData.view_matrix)
		{
			near_plane = cas_near;
			far_plane = cas_far;
			
			mat4 lightView = light.v;

			//Using tanfov calculation
			mat4 invView = camData.view_matrix.inverse();
			

			vec2 near_face = near_plane * camData.tan_halfFOV;
			vec2 far_face = far_plane * camData.tan_halfFOV;

			vec4 frustumEdges[8] =
			{
				// near face
				   vec4{near_face, -near_plane, 1.f},
				   vec4{-near_face.x,  near_face.y, -near_plane, 1.f},
				   vec4{near_face.x,  -near_face.y, -near_plane, 1.f},
				   vec4{-near_face, -near_plane, 1.f},

				   // far face
				   vec4{far_face, -far_plane, 1.f},
				   vec4{-far_face.x,  far_face.y, -far_plane, 1.f},
				   vec4{far_face.x,  -far_face.y, -far_plane, 1.f},
				   vec4{-far_face, -far_plane, 1.f}
			};
			real max_v = std::numeric_limits<float>::max();
			//real min_v = std::numeric_limits<real>::min();
			vec3 min_c{ max_v };
			vec3 max_c = -min_c;

			mat4 m = lightView * invView;	

			for (auto& elem : frustumEdges)
			{
				elem = m * elem;

				min_c = { min(min_c.x,elem.x),min(min_c.y,elem.y) ,min(min_c.z, elem.z) };
				max_c = { max(max_c.x,elem.x),max(max_c.y,elem.y) ,max(max_c.z,elem.z) };
			}

			vec3 max_comp = max(min_c, max_c);
			float max_rad = max(max(max_comp.x, max_comp.y), max_comp.z);

			texel_size = static_cast<unsigned int>(floor((float)cascade_resolution / (2.f * max_rad)));


			//cascade_projection = ortho(-max_rad, max_rad, -max_rad, max_rad, -max_rad, max_rad);
			cascade_projection = ortho(min_c.x, max_c.x, min_c.y, max_c.y, min_c.z, max_c.z);

			cam_max = vec4(0.f, 0.f, far_plane, 1.f);

			oldCamView = camData.view_matrix;
		}
		//clip_plane_z = vClip.z;
	}

	RscHandle<FrameBuffer> Lightmap::InitShadowMap(LightmapConfig config)
	{
		_config = config;
		FrameBufferBuilder builder;
		builder.Begin("ShadowMap[" + std::to_string(texel_size) + ", " + std::to_string(texel_size) + "]", uvec2{ texel_size , texel_size }, config.layer_count);
		builder.SetDepthAttachment(
			AttachmentInfo
			{
				LoadOp::eClear,
				StoreOp::eStore,
				DepthBufferMode::Depth16,
				true,
				FilterMode::_enum::Linear,
				_config.isCubeMap,
				std::nullopt,
				_config.layer_count,
				_config.view_type
			}
		);

		if (light_map)
		{
			light_map->attachments.clear();
			light_map->depth_attachment.reset();
			light_map->stencil_attachment.reset();
		}

		auto& shadow_map = light_map = Core::GetResourceManager().GetFactory<FrameBufferFactory>().Create(builder.End());//Core::GetResourceManager().Create<FrameBuffer>();
		return shadow_map;
	}

	RscHandle<FrameBuffer> Lightmap::InitShadowMap(const size_t& layers, AttachmentViewType type, const bool& isCubeMap)
	{
		return InitShadowMap(LightmapConfig{ layers,type, isCubeMap });
	}
	RscHandle<FrameBuffer> Lightmap::GetShadowMap()
	{
		//if (texel_size != cascade_resolution)
			//return InitShadowMap(_config);

		return light_map;
	}
	LightmapConfig Lightmap::GetConfig() const
	{
		return _config;
	}
	bool Lightmap::NeedLightMap()
	{
		return !light_map;
	}
	void Lightmap::DeleteShadowMap()
	{
		Core::GetResourceManager().Release(light_map);
	}
	unsigned Lightmap::GetShadowMapSize() const
	{
		return texel_size;
	}
}