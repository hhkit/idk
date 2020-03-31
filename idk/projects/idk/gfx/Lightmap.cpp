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
	

	enum class frustum_vert
	{
		eTopLeftNear =0,
		eTopRightNear  ,
		eBotRightNear  ,
		eBotLeftNear   ,
		eTopLeftFar	   ,
		eTopRightFar   ,
		eBotRightFar   ,
		eBotLeftFar	   ,
	};


	/*vec3{-1, 1,-1},//top_left_near  */
	/*vec3{ 1, 1,-1},//top_right_near  */
	/*vec3{ 1,-1,-1},//bot_right_near  */
	/*vec3{-1,-1,-1},//bot_left_near  */
	/*vec3{-1, 1, 1},//top_left_near  */
	/*vec3{ 1, 1, 1},//top_right_near */
	/*vec3{ 1,-1, 1},//bot_right_near */
	/*vec3{-1,-1, 1},//bot_left_near  */
	frustum make_frustum_from_points(span<const vec3> pts) 
	{
		auto top_left_near  = pts[static_cast<size_t>(frustum_vert::eTopLeftNear )];
		auto top_right_near = pts[static_cast<size_t>(frustum_vert::eTopRightNear)];
		auto bot_right_near = pts[static_cast<size_t>(frustum_vert::eBotRightNear)];
		auto bot_left_near  = pts[static_cast<size_t>(frustum_vert::eBotLeftNear )];
		auto top_left_far  = pts[static_cast<size_t>(frustum_vert::eTopLeftFar  )];
		auto top_right_far = pts[static_cast<size_t>(frustum_vert::eTopRightFar )];
		auto bot_right_far = pts[static_cast<size_t>(frustum_vert::eBotRightFar )];
		auto bot_left_far  = pts[static_cast<size_t>(frustum_vert::eBotLeftFar  )];

		auto top_near = top_right_near - top_left_near;
		auto left_near = bot_left_near - top_left_near;
		auto top_left = top_left_far - top_left_near;
		auto top_right = top_right_far - top_right_near;
		auto bot_left = bot_left_far - bot_left_near;

		vec3 near_nml = left_near.cross(top_near).get_normalized();
		vec3 far_nml  = -near_nml;
		vec3 left_nml = top_left.cross(left_near).get_normalized();
		vec3 right_nml = left_near.cross(top_right).get_normalized();
		vec3 top_nml = top_near.cross(top_left).get_normalized();
		vec3 bot_nml = bot_left.cross(top_near).get_normalized();


		std::array<vec4, 6> planes;
		planes[FrustumSide::Left] = vec4{ left_nml,left_nml.dot(top_left_near) };
		planes[FrustumSide::Right] = vec4{ right_nml,right_nml.dot(top_right_near) };
		planes[FrustumSide::Up] = vec4{ top_nml,top_nml.dot(top_right_near) };
		planes[FrustumSide::Down] = vec4{ bot_nml,bot_nml.dot(bot_left_near) };
		planes[FrustumSide::Near] = vec4{ near_nml,near_nml.dot(bot_left_near) };
		planes[FrustumSide::Far] = vec4{ far_nml,far_nml.dot(bot_left_far) };


		frustum result;
		result.sides[FrustumSide::Left] = halfspace{ planes[FrustumSide::Left] };
		result.sides[FrustumSide::Right] = halfspace{ planes[FrustumSide::Right] };
		result.sides[FrustumSide::Down] = halfspace{ planes[FrustumSide::Down] };
		result.sides[FrustumSide::Up] = halfspace{ planes[FrustumSide::Up] };
		result.sides[FrustumSide::Near] = halfspace{ planes[FrustumSide::Near] };
		result.sides[FrustumSide::Far] = halfspace{ planes[FrustumSide::Far] };
		return result;
	}

	int& DbgIndex()
	{
		static int dbg_index = 0;
		return dbg_index;
	}
	color rand_color()
	{
		return color{ (rand() % 101) / 100.0f,(rand() % 101) / 100.0f,(rand() % 101) / 100.0f,1.0f };
	}

	struct CascadeDebug
	{
		static void DebugDraw(span<const vec4> frustum_vertices,vec3 min_c,vec3 max_c,const mat4& invView,const mat4& lightView)
		{
			vec3 frust_points[8];
			for (size_t i = 0; i < frustum_vertices.size(); i++)
			{
				auto& elem = frustum_vertices[i];
				frust_points[i] = invView * elem;
			}

			static color dbg_colors[] = { rand_color() ,rand_color() ,rand_color() ,rand_color() ,rand_color() ,rand_color() ,rand_color() ,rand_color() };
			auto col = dbg_colors[DbgIndex() % std::size(dbg_colors)];
			{

				auto& pts = frust_points;
				auto top_left_near = pts[static_cast<size_t>(frustum_vert::eTopLeftNear)];
				auto top_right_near = pts[static_cast<size_t>(frustum_vert::eTopRightNear)];
				auto bot_right_near = pts[static_cast<size_t>(frustum_vert::eBotRightNear)];
				auto bot_left_near = pts[static_cast<size_t>(frustum_vert::eBotLeftNear)];
				auto top_left_far = pts[static_cast<size_t>(frustum_vert::eTopLeftFar)];
				auto top_right_far = pts[static_cast<size_t>(frustum_vert::eTopRightFar)];
				auto bot_right_far = pts[static_cast<size_t>(frustum_vert::eBotRightFar)];
				auto bot_left_far = pts[static_cast<size_t>(frustum_vert::eBotLeftFar)];


				Core::GetSystem<DebugRenderer>().Draw(top_left_near, top_left_far, col);
				Core::GetSystem<DebugRenderer>().Draw(top_right_near, top_right_far, col);
				Core::GetSystem<DebugRenderer>().Draw(bot_left_near, bot_left_far, col);
				Core::GetSystem<DebugRenderer>().Draw(bot_right_near, bot_right_far, col);

				Core::GetSystem<DebugRenderer>().Draw(top_left_near, top_right_near, col);
				Core::GetSystem<DebugRenderer>().Draw(bot_left_near, bot_right_near, col);
				Core::GetSystem<DebugRenderer>().Draw(bot_left_near, top_left_near, col);
				Core::GetSystem<DebugRenderer>().Draw(bot_right_near, top_right_near, col);

				Core::GetSystem<DebugRenderer>().Draw(top_left_far, top_right_far, col);
				Core::GetSystem<DebugRenderer>().Draw(bot_left_far, bot_right_far, col);
				Core::GetSystem<DebugRenderer>().Draw(bot_left_far, top_left_far, col);
				Core::GetSystem<DebugRenderer>().Draw(bot_right_far, top_right_far, col);

				//Core::GetSystem<DebugRenderer>().Draw(camera_vp_to_frustum(camData.projection_matrix * camData.view_matrix));
				Core::GetSystem<DebugRenderer>().Draw(aabb{ lightView.inverse() * vec4 { min_c,1 },lightView.inverse() * vec4 { max_c,1 } }, col);

			}
		}
	};


	//#pragma optimize("",off)
	void Lightmap::UpdateCascade(const CameraData& camData,LightData& light, float cas_near, float cas_far)
	{
		if (oldCamView != camData.view_matrix)
		{
			near_plane = cas_near;
			far_plane = cas_far;
			
			const mat4 lightView = light.v;

			//Using tanfov calculation
			const mat4 invView = camData.view_matrix_inverse;
			

			vec2 near_face = near_plane * camData.tan_halfFOV;
			vec2 far_face = far_plane * camData.tan_halfFOV;
			
			vec4 frustumEdges[8] =
			{
				// near face
				   vec4{-near_face.x,  near_face.y, -near_plane, 1.f},
				   vec4{near_face, -near_plane, 1.f},
				   vec4{near_face.x,  -near_face.y, -near_plane, 1.f},
				   vec4{-near_face, -near_plane, 1.f},

				   // far face
				   vec4{-far_face.x,  far_face.y, -far_plane, 1.f},
				   vec4{far_face, -far_plane, 1.f},
				   vec4{far_face.x,  -far_face.y, -far_plane, 1.f},
				   vec4{-far_face, -far_plane, 1.f}
			};
			//for (size_t i = 0; i < std::size(frustumEdges); i++)
			//{
			//	frustumEdges[i] = vec4{ frustum_pts[i],1 };
			//}
			constexpr real max_v = std::numeric_limits<float>::max();
			//real min_v = std::numeric_limits<real>::min();
			vec3 min_c{ max_v };
			vec3 max_c = -min_c;

			const mat4 m = lightView * invView;

			vec3 frust_points[8];
			auto ptr = frust_points;


			for (auto& elem : frustumEdges)
			{
				//elem.z *= 0.5f;
				elem = m * elem;
				min_c = { min(min_c.x,elem.x),min(min_c.y,elem.y) ,min(min_c.z, elem.z) };
				max_c = { max(max_c.x,elem.x),max(max_c.y,elem.y) ,max(max_c.z,elem.z) };
				ptr++;
			}

			
			//frustum f = make_frustum_from_points(frust_points);

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
	static array<vec3, 6> cubeMat = {
			vec3{1,0,0},
			vec3{-1,0,0},
			vec3{0,1,0},
			vec3{0,-1,0},
			vec3{0,0,1},
			vec3{0,0,-1}
	};
	static array<int, 6> upDirectionsSpec = {
		3,
		3,
		4,
		5,
		3,
		3
	};
	void Lightmap::UpdatePointMat(const LightData& light)
	{
		if (old_light_pos != light.v_pos)
		{
			old_light_pos = light.v_pos;
			if (shadow_vp.empty())
				shadow_vp.resize(6);
			
			for (int i = 0; i < upDirectionsSpec.size(); ++i)
				shadow_vp[i] = light.p * look_at(light.v_pos, light.v_pos + cubeMat[i], cubeMat[upDirectionsSpec[i]]);
		}
	}
//#pragma optimize("",off)
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