#include "pch.h"
#include "DeferredPass.h"
#include <gfx/FramebufferFactory.h>

#include <vkn/PipelineBinders.h>

#include <vkn/VknRenderTarget.h>
#include <vkn/GraphicsState.h>

#include <vkn/utils/utils.h>

#include <gfx/MeshRenderer.h>
#include <vkn/PipelineBinders.h>
#include <vkn/PipelineBinders.inl>
#include <vkn/FrameRenderer.h>

#include <vkn/VknTextureLoader.h>
#include <vkn/VulkanTextureFactory.h>

#include <vkn/VknTextureRenderMeta.h>

#include <vkn/RenderPassInfo.h>
#include <vkn/VknFrameBufferFactory.h>

#if 1
namespace idk::vkn
{
	using EGBufferBinding = meta::enum_info<GBufferBinding, meta::enum_pack<GBufferBinding,
		GBufferBinding::eAlbedoAmbOcc,
		GBufferBinding::eUvMetallicRoughness,
		GBufferBinding::eViewPos,
		GBufferBinding::eNormal,
		GBufferBinding::eTangent
		>>;

	enum class DeferredBinding
	{
		eMetallicPass ,
		eSpecularPass ,
		eUnlitPass    ,
		eDepths
	};
	using EDeferredBinding = meta::enum_info<DeferredBinding, meta::enum_pack<DeferredBinding,
		DeferredBinding::eMetallicPass,
		DeferredBinding::eSpecularPass,
		DeferredBinding::eUnlitPass   ,
		DeferredBinding::eDepths
		>>;
	struct DeferredBindingIndex
	{
		static constexpr uint32_t fb_buffer_size [EDeferredBinding::size()] = { static_cast<uint32_t>(EDeferredBinding::size()),static_cast<uint32_t>(EDeferredBinding::size()),1,3};
		static constexpr uint32_t begin(DeferredBinding binding)
		{
			uint32_t result = 0;
			for (int i = 0; i < (int)binding; ++i)
			{
				result += fb_buffer_size[i];
			}
			return result;
		}
		static constexpr uint32_t end  (DeferredBinding binding)
		{
			uint32_t result = 0;
			for (int i = 0; i <= (int)binding; ++i)
			{
				result += fb_buffer_size[i];
			}
			return result;
		}
		static constexpr uint32_t size(DeferredBinding binding)
		{
			return fb_buffer_size[(int)binding] ;
		}
	};


	pipeline_config ConfigWithVP(pipeline_config config, const CameraData& camera, const ivec2& offset, const ivec2& size);
	template<typename T, typename...Args>
	using has_setstate = decltype(std::declval<T>().SetState(std::declval<Args>()...));
	PipelineThingy ProcessRoUniforms(const GraphicsState& state, UboManager& ubo_manager, StandardBindings& binders);
	std::pair<ivec2, ivec2> ComputeVulkanViewport(const vec2& sz, const rect& vp);

	void GBufferBarrier(vk::CommandBuffer cmd_buffer, DeferredGBuffer& gbuffer)
	{
		constexpr auto num_buffers = 0;// EGBufferBinding::size();
		std::array<vk::ImageMemoryBarrier, num_buffers + 1> barriers;

		//for (size_t i = 0; i < EGBufferBinding::size(); ++i)
		//{
		//	auto& tex = gbuffer.gbuffer->GetAttachment(i).buffer.as<VknTexture>();
		//	barriers[i] = vk::ImageMemoryBarrier
		//	{
		//		vk::AccessFlagBits::eColorAttachmentWrite,
		//		vk::AccessFlagBits::eShaderRead,
		//		vk::ImageLayout::eColorAttachmentOptimal,
		//		vk::ImageLayout::eShaderReadOnlyOptimal,
		//		*View().QueueFamily().graphics_family,
		//		*View().QueueFamily().graphics_family,
		//		tex.Image(),
		//		vk::ImageSubresourceRange
		//		{
		//			tex.ImageAspects(),
		//			0,1,
		//			0,1,
		//		}
		//	};
		//}
		auto& depth_tex = gbuffer.gbuffer->DepthAttachment().buffer.as<VknTexture>();
		//depth
		barriers[num_buffers] = vk::ImageMemoryBarrier
		{
			{},
			vk::AccessFlagBits::eTransferRead,
			vk::ImageLayout::eGeneral,
			vk::ImageLayout::eTransferSrcOptimal,
			*View().QueueFamily().graphics_family,
			*View().QueueFamily().graphics_family,
			depth_tex.Image(),
			vk::ImageSubresourceRange
			{
				depth_tex.ImageAspects(),
				0,1,
				0,1,
			}
		};
		cmd_buffer.pipelineBarrier(vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eTransfer, vk::DependencyFlagBits::eByRegion, nullptr, nullptr, barriers, vk::DispatchLoaderDefault{});
	}


	vk::Semaphore DeferredGBuffer::RenderCompleteSignal()
	{
		return *_render_complete;
	}


	struct RpBindInfo
	{
		index_t index;
	};
	struct SubFramebuffer
	{
		vector<index_t> color_attachments;
		index_t depth_attachment;
	};


	RenderPassObj ConstructDeferredRenderPass(const FrameBufferInfo& gbuffer)
	{
		RenderPassInfo rp_info;
		RpBindInfo metallic_buffer_indices[DeferredBindingIndex::size(DeferredBinding::eMetallicPass)];
		RpBindInfo specular_buffer_indices[DeferredBindingIndex::size(DeferredBinding::eSpecularPass)];
		RpBindInfo unlit_buffer_indices[DeferredBindingIndex::size(DeferredBinding::eUnlitPass)];
		RpBindInfo depth_buffer_indices[DeferredBindingIndex::size(DeferredBinding::eDepths)];

		auto BindAttachment = [](auto& rp_info, DeferredBinding binding, auto& buffer_indices, auto& attachments)
		{
			auto begin = DeferredBindingIndex::begin(binding);
			for (uint32_t i = 0; i < DeferredBindingIndex::size(binding); ++i)
			{
				buffer_indices[i].index = rp_info.RegisterAttachment(attachments[begin + i], vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);
			}
		};
		auto BindDepth = [](auto& rp_info, DeferredBinding binding, auto& buffer_indices, auto& attachments)
		{
			auto i = (int)binding;
			auto begin = DeferredBindingIndex::begin(DeferredBinding::eDepths);
			buffer_indices[i].index = rp_info.RegisterAttachment(attachments[begin + i], vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);

		};
		BindAttachment(rp_info,DeferredBinding::eMetallicPass, metallic_buffer_indices,gbuffer.attachments);
		BindAttachment(rp_info,DeferredBinding::eSpecularPass, specular_buffer_indices,gbuffer.attachments);
		BindAttachment(rp_info,DeferredBinding::eUnlitPass   , unlit_buffer_indices   ,gbuffer.attachments);

		BindDepth(rp_info, DeferredBinding::eMetallicPass, depth_buffer_indices, gbuffer.attachments);
		BindDepth(rp_info, DeferredBinding::eSpecularPass, depth_buffer_indices, gbuffer.attachments);
		BindDepth(rp_info, DeferredBinding::eUnlitPass   , depth_buffer_indices, gbuffer.attachments);

		//Add all the attachments
		//auto met_alb_idx= rp_info.RegisterAttachment(gbuffer.attachments[(int)GBufferBinding::eAlbedoAmbOcc       ], vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);
		//auto met_nor_idx= rp_info.RegisterAttachment(gbuffer.attachments[(int)GBufferBinding::eNormal             ], vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);
		//auto met_tan_idx= rp_info.RegisterAttachment(gbuffer.attachments[(int)GBufferBinding::eTangent            ], vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);
		//auto met_met_idx= rp_info.RegisterAttachment(gbuffer.attachments[(int)GBufferBinding::eUvMetallicRoughness], vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);
		//auto met_vwp_idx= rp_info.RegisterAttachment(gbuffer.attachments[(int)GBufferBinding::eViewPos            ], vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);
		//auto met_dep_idx= rp_info.RegisterAttachment(*gbuffer.depth_attachment, vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);
		//
		//auto spc_alb_idx = rp_info.RegisterAttachment(gbuffer.attachments[(int)GBufferBinding::eAlbedoAmbOcc], vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);
		//auto spc_nor_idx = rp_info.RegisterAttachment(gbuffer.attachments[(int)GBufferBinding::eNormal], vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);
		//auto spc_tan_idx = rp_info.RegisterAttachment(gbuffer.attachments[(int)GBufferBinding::eTangent], vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);
		//auto spc_spc_idx = rp_info.RegisterAttachment(gbuffer.attachments[(int)GBufferBinding::eUvMetallicRoughness], vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);
		//auto spc_vwp_idx = rp_info.RegisterAttachment(gbuffer.attachments[(int)GBufferBinding::eViewPos], vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);
		//auto spc_dep_idx = rp_info.RegisterAttachment(*gbuffer.depth_attachment, vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);
		//
		//auto unlit_color_idx = rp_info.RegisterAttachment(gbuffer.attachments[(int)GBufferBinding::eAlbedoAmbOcc], vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);
		//auto unlit_depth_idx = rp_info.RegisterAttachment(*gbuffer.depth_attachment, vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);
		//
		//auto combine_color_idx = rp_info.RegisterAttachment(gbuffer.attachments[(int)GBufferBinding::eAlbedoAmbOcc], vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferSrcOptimal);
		//auto combine_depth_idx = rp_info.RegisterAttachment(*gbuffer.depth_attachment, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferSrcOptimal);

		auto AddOutputColors = [](SubPassConfig& config, DeferredBinding binding, auto& buffer_indices)
		{
			for (uint32_t i = 0; i < DeferredBindingIndex::size(binding); ++i)
			{
				config.AddOutputAttachment(buffer_indices[i].index, vk::ImageLayout::eColorAttachmentOptimal);
			}
		};
		auto AddInputColors = [](SubPassConfig& config, DeferredBinding binding, auto& buffer_indices)
		{
			for (uint32_t i = 0; i < DeferredBindingIndex::size(binding); ++i)
			{
				config.AddInputAttachment(buffer_indices[i].index, vk::ImageLayout::eColorAttachmentOptimal);
			}
		};
		auto AddDepthOutput = [](SubPassConfig& config, DeferredBinding binding, auto& depth_indices)
		{
			auto i = (int)binding;
			config.AddOutputAttachment(depth_indices[i].index, vk::ImageLayout::eDepthStencilReadOnlyOptimal);

		};
		auto AddDepthInput = [](SubPassConfig& config, DeferredBinding binding, auto& depth_indices)
		{
			auto i = (int)binding;
			config.AddInputAttachment(depth_indices[i].index, vk::ImageLayout::eDepthStencilReadOnlyOptimal);

		};

		SubPassConfig first_metal_config{};
		AddOutputColors(first_metal_config, DeferredBinding::eMetallicPass, metallic_buffer_indices);
		AddDepthOutput(first_metal_config, DeferredBinding::eMetallicPass, depth_buffer_indices);

		SubPassConfig second_metal_config{};
		AddInputColors(second_metal_config, DeferredBinding::eMetallicPass, metallic_buffer_indices);
		AddDepthInput(second_metal_config, DeferredBinding::eMetallicPass, depth_buffer_indices);
		AddOutputColors(second_metal_config, DeferredBinding::eUnlitPass, unlit_buffer_indices);
		AddDepthOutput(second_metal_config, DeferredBinding::eUnlitPass, depth_buffer_indices);

		SubPassConfig first_specular_config{};
		AddOutputColors(first_specular_config, DeferredBinding::eSpecularPass, specular_buffer_indices);
		AddDepthOutput(first_specular_config, DeferredBinding::eSpecularPass, depth_buffer_indices);

		SubPassConfig second_specular_config{};
		AddInputColors(second_specular_config, DeferredBinding::eSpecularPass, specular_buffer_indices);
		AddDepthInput(second_specular_config, DeferredBinding::eSpecularPass, depth_buffer_indices);
		AddOutputColors(second_specular_config, DeferredBinding::eUnlitPass, unlit_buffer_indices);
		AddDepthOutput(second_specular_config, DeferredBinding::eUnlitPass, depth_buffer_indices);
		
		SubPassConfig unlit_config{};
		AddOutputColors(unlit_config, DeferredBinding::eUnlitPass, unlit_buffer_indices);
		AddDepthOutput(unlit_config, DeferredBinding::eUnlitPass, depth_buffer_indices);


		auto first_metal_pass = rp_info.RegisterSubpass(
			std::move(first_metal_config)
		);
		auto second_metal_pass = rp_info.RegisterSubpass(
			std::move(second_metal_config)
		);

		auto first_specular_pass = rp_info.RegisterSubpass(
			std::move(first_specular_config)
		);
		auto second_specular_pass = rp_info.RegisterSubpass(
			std::move(second_specular_config)
		);


		auto unlit_pass = rp_info.RegisterSubpass(
			std::move(unlit_config)
		);

		//metal    gbuffer
		//specular gbuffer
		//unlit buffer


		rp_info.AddDependency(VK_SUBPASS_EXTERNAL , first_metal_pass     );
		rp_info.AddDependency(VK_SUBPASS_EXTERNAL , first_specular_pass );
		rp_info.AddDependency(VK_SUBPASS_EXTERNAL , unlit_pass          );
		rp_info.AddDependency(unlit_pass          , second_metal_pass   );
		rp_info.AddDependency(second_metal_pass   , second_specular_pass);
		rp_info.AddDependency(first_metal_pass    , second_metal_pass   );
		rp_info.AddDependency(first_specular_pass , second_specular_pass);

		;

		return View().Device()->createRenderPassUnique(rp_info.BuildRenderPass());
	}
	void DeferredGBuffer::Init(ivec2 size)
	{
		if (!gbuffer || gbuffer->Size()!=size)
		{
			FrameBufferBuilder fbf;
			fbf.Begin(size);
			//GBufferBinding::eAlbedoAmbOcc
			fbf.AddAttachment(idk::AttachmentInfo{
				LoadOp::eClear,StoreOp::eStore,
				ColorFormat::_enum::SRGBA,
				FilterMode::_enum::Nearest
				});
			GBufferBinding::eNormal;
			fbf.AddAttachment(idk::AttachmentInfo{
				LoadOp::eClear,StoreOp::eStore,
				ColorFormat::_enum::RGBAF_32,
				FilterMode::_enum::Nearest
				});
			GBufferBinding::eTangent;
			fbf.AddAttachment(idk::AttachmentInfo{
				LoadOp::eClear,StoreOp::eStore,
				ColorFormat::_enum::RGBAF_32,
				FilterMode::_enum::Nearest
				});
			GBufferBinding::eUvMetallicRoughness;
			fbf.AddAttachment(idk::AttachmentInfo{
				LoadOp::eClear,StoreOp::eStore,
				ColorFormat::_enum::RGBAF_32,
				FilterMode::_enum::Nearest
				});
			GBufferBinding::eViewPos;
			fbf.AddAttachment(idk::AttachmentInfo{
				LoadOp::eClear,StoreOp::eStore,
				ColorFormat::_enum::RGBAF_32,
				FilterMode::_enum::Nearest
				});
			fbf.SetDepthAttachment(
				idk::AttachmentInfo{
					LoadOp::eClear,StoreOp::eStore,
					ColorFormat::_enum::DEPTH_COMPONENT,
					FilterMode::_enum::Nearest
				});
			auto fb_info = fbf.End();
			//rp_obj = ConstructDeferredRenderPass(fb_info);
			auto& fb_factory = Core::GetResourceManager().GetFactory<FrameBufferFactory>();

			auto new_buffer = fb_factory.Create(fb_info);
			fbf.Begin(size);
			fbf.AddAttachment(idk::AttachmentInfo{
				LoadOp::eClear,StoreOp::eStore,
				ColorFormat::_enum::RGBAF_32,
				FilterMode::_enum::Nearest
				});
			if (accum_buffer)
				Core::GetResourceManager().Release(accum_buffer);
			accum_buffer = RscHandle<VknFrameBuffer>{ fb_factory.Create(fbf.End()) };
			
			if (gbuffer)
				Core::GetResourceManager().Release(gbuffer);
			gbuffer = RscHandle<VknFrameBuffer>{ new_buffer };
		
			_render_complete = View().Device()->createSemaphoreUnique(vk::SemaphoreCreateInfo{});
		}
	}

	vk::UniqueRenderPass BuildGbufferRenderPass(VknFrameBuffer& fb)
	{
		RenderPassInfo rp_info;
		//Output
		for(size_t i=0; i<EGBufferBinding::size();++i)
			rp_info.RegisterAttachment(fb.GetAttachment(i), vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);
		rp_info.RegisterAttachment(fb.DepthAttachment(), vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);

		rp_info.AddDependency(VK_SUBPASS_EXTERNAL, 0);
		SubPassConfig spc{};
		spc.AddOutputAttachment(0, vk::ImageLayout::eColorAttachmentOptimal);
		spc.BuildSubpass();
		rp_info.RegisterSubpass(spc);
		return View().Device()->createRenderPassUnique(rp_info.BuildRenderPass());
	}
	vk::UniqueRenderPass BuildAccumRenderPass(VknFrameBuffer& fb)
	{
		RenderPassInfo rp_info;
		//Output
		rp_info.RegisterAttachment(fb.GetAttachment(0), vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal);

		SubPassConfig spc{};
		spc.AddOutputAttachment(0, vk::ImageLayout::eColorAttachmentOptimal);
		spc.BuildSubpass();
		rp_info.RegisterSubpass(spc);
		rp_info.AddDependency(VK_SUBPASS_EXTERNAL, 0);
		return View().Device()->createRenderPassUnique(rp_info.BuildRenderPass());
	}
	vk::UniqueRenderPass BuildHdrRenderPass(const FrameBufferInfo& hdr_out, RscHandle<Texture> accum_tex, RscHandle<Texture> depth)
	{
		RenderPassInfo rp_info;

		auto accum = idk::AttachmentInfo{
			LoadOp::eLoad,StoreOp::eDontCare,
			ColorFormat::_enum::SRGB,
			FilterMode::_enum::Nearest,false,
			accum_tex
			};
		auto depth_att = idk::AttachmentInfo{
			LoadOp::eLoad,StoreOp::eDontCare,
			ColorFormat::_enum::DEPTH_COMPONENT,
			FilterMode::_enum::Nearest,false,
			depth
		};

		//Output
		rp_info.RegisterAttachment(hdr_out.attachments[0], vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::eColorAttachmentOptimal);
		rp_info.RegisterAttachment(hdr_out.attachments[1],     vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
		rp_info.RegisterAttachment(hdr_out.attachments[2], vk::ImageLayout::eGeneral, vk::ImageLayout::eShaderReadOnlyOptimal);
		//Input light accum
		//rp_info.RegisterAttachment(accum.GetAttachment(0), vk::ImageLayout::eShaderReadOnlyOptimal, vk::ImageLayout::eGeneral);
		//Input depth (for discard check)
		//rp_info.RegisterAttachment(, vk::ImageLayout::eShaderReadOnlyOptimal, vk::ImageLayout::eGeneral);

		SubPassConfig spc{};
		spc.AddOutputAttachment(0, vk::ImageLayout::eColorAttachmentOptimal);
		spc.AddInputAttachment(1, vk::ImageLayout::eShaderReadOnlyOptimal);
		spc.AddInputAttachment(2, vk::ImageLayout::eShaderReadOnlyOptimal);
		spc.BuildSubpass();

		
		rp_info.RegisterSubpass(spc);
		rp_info.AddDependency(VK_SUBPASS_EXTERNAL,0,
			vk::AccessFlagBits::eColorAttachmentWrite| vk::AccessFlagBits::eDepthStencilAttachmentWrite, 
			vk::AccessFlagBits::eShaderRead, vk::PipelineStageFlagBits::eColorAttachmentOutput| vk::PipelineStageFlagBits::eLateFragmentTests, vk::PipelineStageFlagBits::eFragmentShader );
		return View().Device()->createRenderPassUnique(rp_info.BuildRenderPass());
	}
#pragma optimize("",off)
	void DeferredPass::Init(ivec2 size, VknRenderTarget& rt) 
	{
		const static renderer_attributes fsq_requirements = 
		{
			{
				{vtx::Attrib::Position,0},
				{vtx::Attrib::UV,1},
			}
		};
		
		_gbuffer.Init(rt.Size());

		auto& fb_factory = Core::GetResourceManager().GetFactory<FrameBufferFactory>();
		FrameBufferBuilder fbf;
		fbf.Begin(rt.Size());
		fbf.AddAttachment(idk::AttachmentInfo{
			LoadOp::eLoad,StoreOp::eStore,
			ColorFormat::_enum::SRGBA,
			FilterMode::_enum::Nearest,false,
			rt.GetColorBuffer()
			});
		auto& buffer = *_gbuffer.accum_buffer; 
		auto accum_att = idk::AttachmentInfo{
			LoadOp::eLoad,StoreOp::eDontCare,
			ColorFormat::_enum::SRGB,
			FilterMode::_enum::Nearest,false,
			buffer.GetAttachment(0).buffer
		};
		accum_att.is_input_att = true;
		fbf.AddAttachment(accum_att);
		auto depth_att = idk::AttachmentInfo{
			LoadOp::eLoad,StoreOp::eDontCare,
			ColorFormat::_enum::DEPTH_COMPONENT,
			FilterMode::_enum::Nearest,false,
			_gbuffer.gbuffer->DepthAttachment().buffer
		};
		depth_att.override_as_depth = true;
		depth_att.is_input_att = true;
		fbf.AddAttachment(depth_att);
		if (hdr_buffer)
			Core::GetResourceManager().Release(hdr_buffer);
		auto hdr_info = fbf.End();

/*
		auto& rsm = Core::GetResourceManager();
		if (!depth_sample_tex)
			depth_sample_tex = rsm.Create<VknTexture>();
		if (depth_sample_tex->Size() != size)
		{
			TextureLoader loader;
			auto& tex_factory = Core::GetResourceManager().GetFactory<VulkanTextureFactory>();

			TextureOptions tex_opt{};
			tex_opt.internal_format = ColorFormat::_enum::DEPTH_COMPONENT;
			tex_opt.is_srgb = false;
			auto tex_info = DepthBufferTexInfo(s_cast<uint32_t>(size.x), s_cast<uint32_t>(size.y));
			loader.LoadTexture(*depth_sample_tex, tex_factory.GetAllocator(), tex_factory.GetFence(), tex_opt, tex_info, {});
		}*/

		fsq_light_ro.mesh         = fsq_amb_ro.mesh = Mesh::defaults[MeshType::FSQ];
		fsq_light_ro.renderer_req = fsq_amb_ro.renderer_req = &fsq_requirements;
		if (!fsq_light_ro.config)
		{
			ambient_config = std::make_shared<pipeline_config>(*Core::GetSystem<GraphicsSystem>().MeshRenderConfig());
			light_config = std::make_shared<pipeline_config>(*Core::GetSystem<GraphicsSystem>().MeshRenderConfig());

			light_config->attachment_configs.resize(1);
			auto& blend = light_config->attachment_configs[0];
			blend.blend_enable = true;
			blend.dst_color_blend_factor = BlendFactor::eOne;
			blend.src_color_blend_factor = BlendFactor::eOne;
			blend.color_blend_op = BlendOp::eAdd;
			blend.alpha_blend_op = BlendOp::eMax;
			blend.dst_alpha_blend_factor = BlendFactor::eOne;
			blend.src_alpha_blend_factor = BlendFactor::eOne;
			light_config->depth_test = false;
			fsq_light_ro.config = light_config;
			fsq_amb_ro.config = ambient_config;

			accum_pass = BuildAccumRenderPass(_gbuffer.accum_buffer.as<VknFrameBuffer>());
			hdr_pass = BuildHdrRenderPass(hdr_info, buffer.GetAttachment(0).buffer,_gbuffer.gbuffer->DepthAttachment().buffer);
			//auto test = AttachmentBlendConfig{};
			//test.blend_enable = true;
			//test.alpha_blend_op = BlendOp::eAdd;
			//test.color_blend_op = BlendOp::eAdd;
			//test.src_color_blend_factor = BlendFactor::eSrcAlpha;
			//test.dst_color_blend_factor = BlendFactor::eOneMinusSrcAlpha;
			//test.src_alpha_blend_factor = BlendFactor::eOne;
			//test.dst_alpha_blend_factor = BlendFactor::eOneMinusSrcAlpha; //I swear to god idk why this has to be like this, but otherwise, the blend background is just black. eZero didn't work.
			//blend = test;
		}
		
		auto tmp = VknSpecializedInfo{ hdr_pass };
		hdr_buffer = RscHandle<VknFrameBuffer>{ fb_factory.Create(hdr_info,&tmp) };
	}
	struct DeferredPostBinder : StandardBindings
	{
		bool is_ambient = false;
		DeferredPass* deferred_pass;
		void SetDeferredPass(DeferredPass& pass)
		{
			deferred_pass = &pass;
		}
		void Bind(PipelineThingy& the_interface) override
		{
			RscHandle<ShaderProgram> fsq_vert = deferred_pass->fullscreen_quad_vert;
			RscHandle<ShaderProgram> deferred_post_frag = deferred_pass->deferred_post_frag;
			RscHandle<ShaderProgram> deferred_post_ambient = deferred_pass->deferred_post_ambient;

			the_interface.BindShader(ShaderStage::Vertex, fsq_vert);
			the_interface.BindShader(ShaderStage::Fragment, (is_ambient)? deferred_post_ambient:deferred_post_frag);
			auto& gbuffer_fb = deferred_pass->GBuffer().gbuffer;
			for (uint32_t i = 0; i < EGBufferBinding::size(); ++i)
				the_interface.BindSampler("gbuffers", i, gbuffer_fb->GetAttachment(i).buffer.as<VknTexture>());
			the_interface.BindSampler("gbuffers", static_cast<uint32_t>(EGBufferBinding::size()), gbuffer_fb->DepthAttachment().buffer.as<VknTexture>(),false,vk::ImageLayout::eGeneral);
		}
	};


	struct DeferredHdrBinder : StandardBindings
	{
		bool is_ambient = false;
		DeferredPass* deferred_pass;
		void SetDeferredPass(DeferredPass& pass)
		{
			deferred_pass = &pass;
		}
		void Bind(PipelineThingy& the_interface) override
		{
			RscHandle<ShaderProgram> fsq_vert = deferred_pass->fullscreen_quad_vert;
			RscHandle<ShaderProgram> hdr_frag = deferred_pass->hdr_frag;

			the_interface.BindShader(ShaderStage::Vertex, fsq_vert);
			the_interface.BindShader(ShaderStage::Fragment, hdr_frag);
		}
	};

	using PbrDeferredPostBinding = CombinedBindings<DeferredPostBinder, PbrFwdBindings>;

	void DeferredPass::LightPass(PipelineThingy& the_interface, const GraphicsState& graphics_state, RenderStateV2& rs, std::optional<std::pair<size_t, size_t>> light_range,bool is_ambient)
	{
		//TODO: Prepare FSQ draw call + Forward Draw Calls
		PbrDeferredPostBinding binding;
		auto& def_bind = std::get<DeferredPostBinder>(binding.binders);
		def_bind.SetDeferredPass(*this);
		def_bind.is_ambient = is_ambient;
		auto& pbr_fwd_binding = std::get<PbrFwdBindings>(binding.binders);
		pbr_fwd_binding.light_range = light_range;
		pbr_fwd_binding.rebind_light = true;
		pbr_fwd_binding.SetState(graphics_state);

		auto& fsq_ro = (is_ambient) ? fsq_amb_ro : fsq_light_ro;

		binding.Bind(the_interface);
		binding.Bind(the_interface, fsq_ro);
		the_interface.BindMeshBuffers(fsq_ro);

		//Draw Fullscreen Quad
		the_interface.FinalizeDrawCall(fsq_ro);
		//Insert Forward stuff here?

	}

	PipelineThingy DeferredPass::HdrPass(const GraphicsState& graphics_state, RenderStateV2& rs)
	{
		PipelineThingy the_interface{};
		the_interface.SetRef(rs.ubo_manager);


		DeferredHdrBinder binding;
		auto& def_bind = binding;
		def_bind.SetDeferredPass(*this);

		auto& fsq_ro = fsq_amb_ro ;

		binding.Bind(the_interface);
		auto& buffer = *hdr_buffer;
		auto& gbuffer = _gbuffer.gbuffer.as<VknFrameBuffer>();
		the_interface.BindAttachment("light_accum_input", 0, buffer.GetAttachment(1).buffer.as<VknTexture>(), false, vk::ImageLayout::eShaderReadOnlyOptimal);
		the_interface.BindAttachment("depth_input", 0, buffer.GetAttachment(2).buffer.as<VknTexture>(),false,vk::ImageLayout::eGeneral);
		the_interface.BindMeshBuffers(fsq_ro);

		//Draw Fullscreen Quad
		the_interface.FinalizeDrawCall(fsq_ro);

		return the_interface;
	}

	//
	/*
	PipelineThingy DeferredPass::ProcessDrawCalls(const GraphicsState& graphics_state, RenderStateV2& rs, std::optional<std::pair<size_t, size_t>>light_range)
	{
		//TODO: Prepare FSQ draw call + Forward Draw Calls
		PipelineThingy the_interface{};
		the_interface.SetRef(rs.ubo_manager);
		PbrDeferredPostBinding binding;
		std::get<DeferredPostBinder>(binding.binders).SetDeferredPass(*this);
		auto& pbr_fwd_binding = std::get<PbrFwdBindings>(binding.binders); 
		pbr_fwd_binding.light_range = light_range;
		pbr_fwd_binding.SetState(graphics_state);

		auto& fsq_ro = fsq_amb_ro ;
		
		binding.Bind(the_interface);
		binding.Bind(the_interface, fsq_ro);
		the_interface.BindMeshBuffers(fsq_ro);
		//Draw Fullscreen Quad
		the_interface.FinalizeDrawCall(fsq_ro);
		//Insert Forward stuff here?

		return the_interface;
	}
	*/

	void PostRenderCopy(vk::CommandBuffer cmd_buffer,ivec2 size, vk::Image render_target_depth, vk::Image gbuffer_depth)
	{
		//Transit RTD -> Transfer
		vk::ImageMemoryBarrier depth
		{
			{},
			vk::AccessFlagBits::eTransferWrite,
			vk::ImageLayout::eDepthStencilAttachmentOptimal,
			vk::ImageLayout::eTransferDstOptimal,
			*View().QueueFamily().graphics_family,
			*View().QueueFamily().graphics_family,
			render_target_depth,
			vk::ImageSubresourceRange
			{
				vk::ImageAspectFlagBits::eDepth,
				0,1,0,1
			}
		};
		std::array<vk::ImageMemoryBarrier, 2> convert_barriers = { depth,depth };
		convert_barriers[1].setDstAccessMask(vk::AccessFlagBits::eTransferRead);
		convert_barriers[1].setOldLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
		convert_barriers[1].setNewLayout(vk::ImageLayout::eTransferSrcOptimal);
		convert_barriers[1].setImage(gbuffer_depth);
		cmd_buffer.pipelineBarrier(vk::PipelineStageFlagBits::eBottomOfPipe, vk::PipelineStageFlagBits::eTransfer, vk::DependencyFlagBits::eByRegion, nullptr, nullptr, convert_barriers);

		//Blit
		//expect depth attachments to be the exact same size and format.
		cmd_buffer.copyImage(
			gbuffer_depth, vk::ImageLayout::eTransferSrcOptimal,
			render_target_depth, vk::ImageLayout::eTransferDstOptimal,
			vk::ImageCopy
			{
				vk::ImageSubresourceLayers
				{
					vk::ImageAspectFlagBits::eDepth,
					0u,0u,1u
				},
				vk::Offset3D{0,0,0},
				vk::ImageSubresourceLayers
				{
					vk::ImageAspectFlagBits::eDepth,
					0ui32,0ui32,1ui32
				},
				vk::Offset3D{0,0,0},
				vk::Extent3D{s_cast<uint32_t>(size.x),s_cast<uint32_t>(size.y)}
			}
		);

		//Transit RTD -> General
		std::array<vk::ImageMemoryBarrier, 2> & return_barriers = convert_barriers;// { depth, depth };
		std::swap(return_barriers[0].dstAccessMask, return_barriers[0].srcAccessMask);
		std::swap(return_barriers[0].oldLayout, return_barriers[0].newLayout);
		return_barriers[0].setDstAccessMask({});// vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite);
		return_barriers[0].setNewLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

		//std::swap(return_barriers[2].dstAccessMask, return_barriers[2].srcAccessMask);
		//std::swap(return_barriers[2].oldLayout, return_barriers[2].newLayout);
		//return_barriers[2].setDstAccessMask({});// vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite);
		//return_barriers[2].setNewLayout(vk::ImageLayout::eGeneral);

		//Transit GD  -> DepthAttachmentOptimal
		return_barriers[1].setSrcAccessMask(vk::AccessFlagBits::eTransferRead);
		return_barriers[1].setDstAccessMask({});
		return_barriers[1].setOldLayout(vk::ImageLayout::eTransferSrcOptimal);
		return_barriers[1].setNewLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
		return_barriers[1].setImage(gbuffer_depth);



		cmd_buffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eEarlyFragmentTests, vk::DependencyFlagBits::eByRegion, nullptr, nullptr, return_barriers);

	}

	bool RenderProcessedDrawCalls(vk::CommandBuffer cmd_buffer,
		const vector<ProcessedRO>& processed_ro,
		const CameraData& camera,
		PipelineManager& pipeline_manager,
		uint32_t frame_index,
		const vk::RenderPassBeginInfo& rpbi,
		RenderPassObj render_pass,
		uint32_t num_attachments,
		const GraphicsState* state = nullptr
		)
	{
		auto offset = ivec2{ rpbi.renderArea.offset.x,rpbi.renderArea.offset.y };
		auto size= ivec2{ s_cast<uint32_t>(rpbi.renderArea.extent.width),s_cast<uint32_t>(rpbi.renderArea.extent.height )};
		auto& view = View();
		vector<RscHandle<ShaderProgram>> shaders;
		VulkanPipeline* prev_pipeline = nullptr;
		//Draw stuff into the gbuffers
		//auto& processed_ro = the_interface.DrawCalls();
		bool rendered = false;
		for (auto& p_ro : processed_ro)
		{
			rendered = true;
			auto& obj = p_ro.Object();

			bool is_mesh_renderer = p_ro.vertex_shader == Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VNormalMesh];
			if (p_ro.rebind_shaders)
			{
				shaders.resize(0);
				if (p_ro.frag_shader)
					shaders.emplace_back(*p_ro.frag_shader);
				if (p_ro.vertex_shader)
					shaders.emplace_back(*p_ro.vertex_shader);
				if (p_ro.geom_shader)
					shaders.emplace_back(*p_ro.geom_shader);
				
				auto config = ConfigWithVP(*obj.config, camera, offset, size);
				config.attachment_configs.resize(num_attachments);
				if (is_mesh_renderer)
					config.buffer_descriptions.emplace_back(
						buffer_desc
						{
							buffer_desc::binding_info{ std::nullopt,sizeof(mat4) * 2,VertexRate::eInstance},
							{buffer_desc::attribute_info{AttribFormat::eMat4,4,0,true},
							 buffer_desc::attribute_info{AttribFormat::eMat4,8,sizeof(mat4),true}
							 }
						}
				);
				auto& pipeline = pipeline_manager.GetPipeline(config, shaders, frame_index, render_pass, true);
				
				pipeline.Bind(cmd_buffer, view);
				SetViewport(cmd_buffer, offset, size);
				prev_pipeline = &pipeline;
			}
			auto& pipeline = *prev_pipeline;
			//TODO Grab everything and render them
			//auto& mat = obj.material_instance.material.as<VulkanMaterial>();
			auto& mesh = obj.mesh.as<VulkanMesh>();
			{
				uint32_t set = 0;
				for (auto& ods : p_ro.descriptor_sets)
				{
					if (ods)
					{
						auto& ds = *ods;
						cmd_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *pipeline.pipelinelayout, set, ds, {});
					}
					++set;
				}
			}
			auto& renderer_req = *obj.renderer_req;

			for (auto&& [attrib, location] : renderer_req.mesh_requirements)
			{
				auto& attrib_buffer = mesh.Get(attrib);
				cmd_buffer.bindVertexBuffers(*pipeline.GetBinding(location), *attrib_buffer.buffer(), vk::DeviceSize{ attrib_buffer.offset }, vk::DispatchLoaderDefault{});
			}

			if (state&&is_mesh_renderer)
			{
				uint32_t obj_trf_loc = 4;
				cmd_buffer.bindVertexBuffers(*pipeline.GetBinding(obj_trf_loc), state->shared_gfx_state->inst_mesh_render_buffer.buffer(), { 0 }, vk::DispatchLoaderDefault{});
			}
			auto& oidx = mesh.GetIndexBuffer();
			if (oidx)
			{
				cmd_buffer.bindIndexBuffer(*(*oidx).buffer(), 0, mesh.IndexType(), vk::DispatchLoaderDefault{});
				cmd_buffer.drawIndexed(mesh.IndexCount(), static_cast<uint32_t>(p_ro.num_instances), 0, 0, static_cast<uint32_t>(p_ro.inst_offset), vk::DispatchLoaderDefault{});
			}
		}
		return rendered;
	}
	
	void DeferredPass::DrawToGBuffers(vk::CommandBuffer cmd_buffer,const GraphicsState& graphics_state,RenderStateV2& rs)
	{
		auto& gbuffer = GBuffer();
		//Bind the material uniforms
		PbrFwdMaterialBinding binder;
		binder.for_each_binder<has_setstate>([](auto& binder, const GraphicsState& state) { binder.SetState(state); }, graphics_state);

		//Preprocess MeshRender's uniforms
		auto&& the_interface = ProcessRoUniforms(graphics_state, rs.ubo_manager, binder);
		the_interface.GenerateDS(rs.dpools,false);



		std::array<float, 4> a{};

		//auto& cd = std::get<vec4>(state.camera.clear_data);
		//TODO grab the appropriate framebuffer and begin renderpass

		//auto& vvv = state.camera.render_target.as<VknFrameBuffer>();

		auto& camera = graphics_state.camera;
		//auto default_frame_buffer = *swapchain.frame_buffers[swapchain.curr_index];
		auto& g_buffer = *gbuffer.gbuffer;
		auto frame_buffer = g_buffer.GetFramebuffer();
		//TransitionFrameBuffer(camera, cmd_buffer, view);

		auto sz = g_buffer.Size();
		rect viewport{};
		auto [offset, size] = ComputeVulkanViewport(vec2{ sz }, viewport);

		std::array<float, 4> depth_clear{ 1.0f,1.0f ,1.0f ,1.0f };
		std::array<float, 4> g_clear{ 0.0f,0.0f ,0.0f ,0.0f };

		vk::ClearValue v[EGBufferBinding::size()+1]{};
		for(auto& value : v)
		{
			value = vk::ClearValue{ g_clear };
		};
		v[EGBufferBinding::size()] =vk::ClearValue{ vk::ClearColorValue{ depth_clear } };

		vk::Rect2D render_area
		{
			vk::Offset2D
			{
				s_cast<int32_t>(offset.x),s_cast<int32_t>(offset.y)
			},vk::Extent2D
			{
				s_cast<uint32_t>(size.x),s_cast<uint32_t>(size.y)
			}
		};
		auto& rp = g_buffer.GetRenderPass();
		vk::RenderPassBeginInfo rpbi
		{
			*rp, frame_buffer,
			render_area,hlp::arr_count(v),std::data(v)
		};
		cmd_buffer.beginRenderPass(rpbi, vk::SubpassContents::eInline);
		if (RenderProcessedDrawCalls(cmd_buffer, the_interface.DrawCalls(), camera, pipeline_manager(), frame_index(), rpbi,rp,static_cast<uint32_t>(g_buffer.NumColorAttachments()),&graphics_state))
			rs.FlagRendered();
		cmd_buffer.endRenderPass();//End GBuffer pass
		//GBufferBarrier(cmd_buffer, gbuffer);
	}

	void DeferredPass::DrawToAccum(vk::CommandBuffer cmd_buffer, PipelineThingy& accum_stuff, const CameraData& camera, RenderStateV2& rs)
	{
		auto sz = camera.render_target->Size();
		auto [offset, size] = ComputeVulkanViewport(vec2{ sz }, camera.viewport);
		vk::Rect2D render_area
		{
			vk::Offset2D
			{
				s_cast<int32_t>(offset.x),s_cast<int32_t>(offset.y)
			},vk::Extent2D
			{
				s_cast<uint32_t>(size.x),s_cast<uint32_t>(size.y)
			}
		};
		vk::ClearValue v[2]
		{
			vk::ClearColorValue{std::array<float,4>{0.0f,0.0f,0.0f,0.0f}},
			vk::ClearDepthStencilValue{1,0},
		};

		const auto& rp = accum_pass;
		vk::RenderPassBeginInfo rpbi
		{
			*rp, _gbuffer.accum_buffer.as<VknFrameBuffer>().GetFramebuffer(),
			render_area,hlp::arr_count(v),std::data(v)
		};

		//Transit depth buffer to general for sampling


		//Begin Depthless RT renderpass
		cmd_buffer.beginRenderPass(rpbi, vk::SubpassContents::eInline);
		//Draw FSQ
		RenderProcessedDrawCalls(cmd_buffer, accum_stuff.DrawCalls(), camera, pipeline_manager(), frame_index(), rpbi, rp, 1);
		//End Depthless RT renderpass
		cmd_buffer.endRenderPass();
		//Copy depth buffer to render target's depth buffer

	}

	void DeferredPass::DrawToRenderTarget(vk::CommandBuffer cmd_buffer, PipelineThingy& fsq_stuff,const CameraData& camera, VknRenderTarget& rt, [[maybe_unused]]RenderStateV2& rs)
	{
		auto sz = camera.render_target->Size();
		auto [offset, size] = ComputeVulkanViewport(vec2{ sz }, camera.viewport);
		vk::Rect2D render_area
		{
			vk::Offset2D
			{
				s_cast<int32_t>(offset.x),s_cast<int32_t>(offset.y)
			},vk::Extent2D
			{
				s_cast<uint32_t>(size.x),s_cast<uint32_t>(size.y)
			}
		};
		vk::ClearValue v[2]
		{
			vk::ClearColorValue{std::array<float,4>{0.0f,0.0f,0.0f,0.0f}},
			vk::ClearDepthStencilValue{1,0},
		};
		
		const auto& rp = hdr_pass;
		vk::RenderPassBeginInfo rpbi
		{
			*rp, hdr_buffer->GetFramebuffer(),
			render_area,hlp::arr_count(v),std::data(v)
		};

		//Transit depth buffer to general for sampling
		
		//Begin Depthless RT renderpass
		cmd_buffer.beginRenderPass(rpbi,vk::SubpassContents::eInline);
		//Draw FSQ
		RenderProcessedDrawCalls(cmd_buffer, fsq_stuff.DrawCalls(), camera, pipeline_manager(), frame_index(),rpbi,rp,1);
		//End Depthless RT renderpass
		cmd_buffer.endRenderPass();
		//Copy depth buffer to render target's depth buffer
		auto gbuffer_depth_img = GBuffer().gbuffer->DepthAttachment()->as<VknTexture>().Image();
		PostRenderCopy(cmd_buffer, ivec2{ s_cast<int>(render_area.extent.width),s_cast<int>(render_area.extent.height) },rt.GetDepthBuffer().as<VknTexture>().Image(), gbuffer_depth_img);
	}
}
#endif