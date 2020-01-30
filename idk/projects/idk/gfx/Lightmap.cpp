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
	void Lightmap::SetCascade(const CameraData& camData,LightData& light, float cas_near, float cas_far)
	{
		near_plane = cas_near;
		far_plane = cas_far;
		//light.camDataRef = camData;

		real end = cas_near + cas_far;
		vec3 new_center =  camData.pos + camData.look * (near_plane + 0.5f * end);

		vec3 bound = camData.pos + camData.far_corner * far_plane - new_center;
		bound_radius = max(bound.length(),bound_radius);

		texel_size = static_cast<unsigned int>(floor((float)cascade_resolution / (2.f * bound_radius)));

		vec3 cascadeTranslation = new_center - center;

		//center = new_center;

		vec3 center_offset = cascadeTranslation * (float)texel_size;

		//Update if changes are huge
		if (abs(center_offset.x) > 0.5f || abs(center_offset.y) > 0.5f)
		{
			vec3 another_center_offset;
			another_center_offset.z = cascadeTranslation.z;
			another_center_offset.xy = vec2{floor(center_offset.x + 0.5f) / (real)texel_size,floor(center_offset.x + 0.5f) / (real)texel_size };

			//World space conversion from light space
			another_center_offset = light.v.inverse() * vec4 { another_center_offset, 1.f };
			center += another_center_offset.xyz;
		}

		//Shadow space center
		center = ((light.v * camData.tight_projection_matrix) * vec4(center, 1.f)).xyz;

		//transform current ortho proj to cascade world ortho proj

		//mat4 trans{ identity<mat4>() }, scale_mat{ identity<mat4>() };

		//trans = translate(trans, -center + -center);

		//real scale_ratio = camData.b_sphere_radius / bound_radius;

		////scale = scale(scale, scale_ratio);
		//scale_mat = scale(vec3(scale_ratio, scale_ratio, 1.f));


		//cascade_projection = camData.tight_projection_matrix * trans * scale_mat;

		//vec3 focus_point = light.camDataRef.projection_center + light.v_dir.get_normalized() * light.camDataRef.far_plane;
		//vec3 up_vector = light.v_dir.cross(vec3(1.f, 0, 0)).normalize();

		//light.v = look_at(light.camDataRef.projection_center, focus_point, up_vector).inverse();

		mat4 invView = camData.view_matrix.inverse();
		mat4& lightView = light.v;

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
		vec3 min_c = { max_v ,max_v ,max_v };
		vec3 max_c = -min_c;
		vec3 mmc, mmv, v;

		mmc = min_c;
		mmv = max_c;

		mat4 m = lightView * invView;

		for (auto& elem : frustumEdges)
		{
			v = invView * elem;
			elem =  m * elem;
			//elem = vw;

			min_c = {min(min_c.x,elem.x),min(min_c.y,elem.y) ,min(min_c.z,elem.z) };
			max_c = { max(max_c.x,elem.x),max(max_c.y,elem.y) ,max(max_c.z,elem.z) };
			mmc = { min(mmc.x,v.x),min(mmc.y,v.y) ,min(mmc.z,v.z) };
			mmv = { max(mmv.x,v.x),max(mmv.y,v.y) ,max(mmv.z,v.z) };
		}
		Core::GetSystem<DebugRenderer>().Draw(aabb{ mmc, mmv }, color{ 1,1,1,1 }, seconds(0.5f));
		//vec4 vView(0.0f, 0.0f, -far_plane, 1.0f);
		//vec4 vClip = camData.projection_matrix * vView;
		//bound_radius = max(max(min_c.x + max_c.x,abs(max_c.y - min_c.y)),min_c.z + max_c.z);

		//texel_size = static_cast<unsigned int>(floor((float)cascade_resolution / ( bound_radius)));

		cascade_projection = ortho(min_c.x, max_c.x, min_c.y, max_c.y,min_c.z,max_c.z);
		//clip_plane_z = vClip.z;
	}

	RscHandle<FrameBuffer> Lightmap::InitShadowMap()
	{

		FrameBufferBuilder builder;
		builder.Begin("ShadowMap[" + std::to_string(texel_size) + ", " + std::to_string(texel_size) + "]", ivec2{ texel_size , texel_size});
		builder.SetDepthAttachment(
			AttachmentInfo
			{
				LoadOp::eClear,
				StoreOp::eStore,
				DepthBufferMode::Depth16,
				false,
				FilterMode::_enum::Linear
			}
		);
		auto& shadow_map = light_map = Core::GetResourceManager().GetFactory<FrameBufferFactory>().Create(builder.End());//Core::GetResourceManager().Create<FrameBuffer>();
		return shadow_map;
	}
	RscHandle<FrameBuffer> Lightmap::GetShadowMap()
	{
		//if (texel_size != cascade_resolution)
			//return InitShadowMap();

		return light_map;
	}
	bool Lightmap::NeedLightMap()
	{
		return !light_map;
	}
	unsigned Lightmap::GetShadowMapSize() const
	{
		return texel_size;
	}
}