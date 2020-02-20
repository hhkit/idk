#pragma once
#include <vkn/RenderBindings.h>
#include <vkn/CameraViewportBindings.h>
#include <vkn/RenderUtil.h>
namespace idk::vkn::bindings
{
	class AdditiveBlendBindings :public RenderBindings
	{
	public:
		void Bind(RenderInterface& context)override
		{
			size_t i = 0;

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
		static constexpr size_t stride = 8;
		void Bind(RenderInterface& context)override
		{
			auto pbr_trf = _pbr_trf;
			context.BindUniform("PBRBlock", 0, string_view{ hlp::buffer_data<const char*>(pbr_trf),hlp::buffer_size(pbr_trf) });
		}
		bool Skip(RenderInterface& context, const RenderObject& ro) override
		{
			return i + stride > _light_indices.size();
		}
		void Bind(RenderInterface& context, const RenderObject&)
		{
			auto light_indices = _light_indices;
			vector<LightData> lights;
			vector<VknTextureView> shadow_maps;
			lights.clear();
			for (size_t j = 0; j + i < light_indices.size() && j < stride; ++j)
			{
				lights.emplace_back((all_lights)[light_indices[i + j]]);
				context.BindUniform("shadow_maps", j, all_shadows[i + j].as<VknTexture>());
			}

			auto light_data = PrepareLightBlock(_view_matrix, lights);
			context.BindUniform("LightBlock", 0, light_data);
			i += 8;
		}
	private:
		size_t i = 0;
		span<size_t> _light_indices;
		span<LightData> all_lights;
		span<RscHandle<Texture>> all_shadows;
		mat4 _view_matrix;
		FakeMat4<float> _pbr_trf;
	};
	using LightBind = CombinedBindings<AdditiveBlendBindings, CameraViewportBindings, LightShadowBinding>;
}