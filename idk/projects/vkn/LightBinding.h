#pragma once
#include <vkn/RenderBindings.h>
#include <vkn/CameraViewportBindings.h>
#include <vkn/RenderUtil.h>

#include <vkn/VulkanWin32GraphicsSystem.h>
namespace idk::vkn::bindings
{
	class AdditiveBlendBindings :public RenderBindings
	{
	public:
		void Bind(RenderInterface& context)override
		{
			uint32_t i = 0;

			AttachmentBlendConfig blend{};
			blend.blend_enable = true;
			blend.dst_color_blend_factor = BlendFactor::eOne;
			blend.src_color_blend_factor = BlendFactor::eOne;
			blend.color_blend_op = BlendOp::eAdd;
			blend.alpha_blend_op = BlendOp::eMax;
			blend.dst_alpha_blend_factor = BlendFactor::eOne;
			blend.src_alpha_blend_factor = BlendFactor::eOne;
			context.SetBlend(i, blend);
			context.SetClearColor(i, idk::color{ 0,0,0,0 });
			context.SetClearDepthStencil(1.0f);
		}
	};
	class LightShadowBinding: public RenderBindings
	{
	public:
		struct DLightData {
			float far_plane{};
			mat4 vp{};
		};
		struct ShaderDirectionalData
		{
			alignas(16) float far_plane;
			FakeMat4<float> vp;
			ShaderDirectionalData() = default;
			ShaderDirectionalData(const DLightData& data) : vp{ data.vp }, far_plane{ data.far_plane } {}
		};
		static constexpr size_t stride = 8;
		void Bind(RenderInterface& context)override
		{
			auto pbr_trf = _state._pbr_trf;
			context.BindUniform("PBRBlock", 0, string_view{ hlp::buffer_data<const char*>(pbr_trf),hlp::buffer_size(pbr_trf) });
		}
		bool Skip([[maybe_unused]] RenderInterface& context, [[maybe_unused]] const RenderObject& ro) override
		{
			return i  >= _state._light_indices.size();
		}


		static void BindShadows(RenderInterface& context,string_view name, span<const VknTextureView> shadows)
		{
			uint32_t i = 0;
			if (shadows.size() == 0)
			{
				//Make sure that it's there.
				auto& tex = RscHandle<Texture>{}.as<VknTexture>();
				context.BindUniform(name, 0, tex);
			}
			else
			{
				//Bind the shadow maps
				for (auto& shadow_map : shadows)
				{
					//auto& sm_uni = shadow_map;
					{
						auto& depth_tex = shadow_map;
						context.BindUniform(name,i++, depth_tex);
					}
				}
			}

		}

		static string PrepareDirectionalBlock(const vector<DLightData>& vp)
		{
			vector<ShaderDirectionalData> tmp_dlight(std::max(1ui64,vp.size()));
			for (size_t i = 0; i < vp.size(); ++i)
			{
				tmp_dlight[i] = vp[i];
			}
			string d_block;
			d_block += string{ reinterpret_cast<const char*>(tmp_dlight.data()), hlp::buffer_size(tmp_dlight) };
			return d_block;
		}

		void Bind(RenderInterface& context, const RenderObject&)
		{
			auto light_indices = _state._light_indices;
			auto all_lights = _state.all_lights;
			auto all_shadows = _state.all_shadows;
			auto _view_matrix = _state._view_matrix;
			vector<LightData> lights;
			vector<VknTextureView> shadow_maps;
			vector<VknTextureView> shadow_maps_directional;
			vector<VknTextureView> shadow_maps_point;
			vector<DLightData> directional_vp{};
			mat4 clip_mat = mat4{ vec4{1,0,0,0},vec4{0,1,0,0},vec4{0,0,0.5f,0},vec4{0,0,0.5f,1} };
			lights.clear();
			for (size_t j = 0; j + i < light_indices.size() && j < stride; ++j)
			{
				auto& light = all_lights[light_indices[i + j]];
				lights.emplace_back(light);
				
				if (light.index == 0)
				{
					if (!light.light_maps.empty())
						shadow_maps.emplace_back(*RscHandle<VknTexture>{});

					for (auto& elem : light.light_maps)
					{
						auto& fb = elem.light_map.as<VknFrameBuffer>();
						auto& db = fb.DepthAttachment();
						auto& v = db.buffer;
						shadow_maps_point.emplace_back(v.as<VknTexture>());
					}
				}
				else if (light.index == 1)
				{
					if (!light.light_maps.empty())
						shadow_maps.emplace_back(*RscHandle<VknTexture>{});

					for (auto& elem : light.light_maps)//state.d_lightmaps->at(cam.obj_id).cam_lightmaps)
					{
						shadow_maps_directional.emplace_back(elem.light_map.as<VknFrameBuffer>().DepthAttachment().buffer.as<VknTexture>());
						directional_vp.emplace_back(DLightData{ elem.cam_max.z,clip_mat * elem.cascade_projection * light.v });
					}
				}
				else if (light.index == 2)
				{
					for (auto& elem : light.light_maps)
					{
						auto& fb = elem.light_map.as<VknFrameBuffer>();
						auto& db = fb.DepthAttachment();
						auto& v = db.buffer;
						shadow_maps.emplace_back(v.as<VknTexture>());
					}
				}
			}
			Bind(context);

			auto light_data = PrepareLightBlock(_view_matrix, lights);
			auto dlight_data = PrepareDirectionalBlock(directional_vp);
			context.BindUniform("LightBlock", 0, light_data);
			context.BindUniform("DirectionalBlock", 0, dlight_data);

			BindShadows(context, "shadow_map_directional", shadow_maps_directional);
			BindShadows(context, "shadow_map_point", shadow_maps_point);
			BindShadows(context, "shadow_maps", shadow_maps);

			i += stride;
		}
		struct State
		{
			span<const size_t> _light_indices;
			span<const LightData> all_lights;
			span<const RscHandle<Texture>> all_shadows;
			mat4 _view_matrix;
			FakeMat4<float> _pbr_trf;
		};
		void SetState(State state)
		{
			_state = state;
		}
	private:
		size_t i = 0;
		State _state;
	};
	class DeferredLightFsq : public RenderBindings
	{
	public:
		void Bind(RenderInterface& context)override;

		void SetCamera(CameraData cam, RscHandle<Texture> brdfLookupTable)
		{
			camera = cam ;
			brdf_lut = brdfLookupTable;
		}
		CameraData camera;
		RscHandle<Texture> brdf_lut;
		RscHandle<ShaderProgram> fragment_shader;

	};
	class DeferredAmbientLight : public RenderBindings
	{
	public:
		bool Skip(RenderInterface& context, const RenderObject&) override;
		void Bind(RenderInterface& context)override;

		void SetCamera(CameraData cam, RscHandle<Texture> brdfLookupTable)
		{
			camera = cam;
			brdf_lut = brdfLookupTable;
		}
		DeferredAmbientLight();
		CameraData camera;
		RscHandle<Texture> brdf_lut;
		RscHandle<ShaderProgram> fragment_shader;
	};
	using AmbientBind = DeferredAmbientLight;// CombinedBindings<AdditiveBlendBindings, CameraViewportBindings, DeferredAmbientLight>;
	using LightBind = CombinedBindings<AdditiveBlendBindings, CameraViewportBindings, DeferredLightFsq, LightShadowBinding>;
}