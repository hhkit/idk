#include "pch.h"
#include "VknFramebufferFactory.h"
#include "vkn/VknFrameBuffer.h"
#include <vkn/VknTexture.h>
#include <vkn/VknTextureLoader.h>
#include <vkn/VulkanView.h>
#include <vkn/BufferHelpers.h>
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>
#include <res/Guid.inl>
#include <res/ResourceMeta.inl>

struct AttachmentOps
{
	unsigned load   :  2;//take up 2bits
	unsigned store  :  1;//take up 1bits
	unsigned sload  :  2;//take up 2bits
	unsigned sstore :  1;//take up 1bits
	AttachmentOps(const idk::Attachment& a):
		load{ static_cast<decltype(load)>(a.load_op) },
		store{ static_cast<decltype(store)>(a.store_op) },
		sload{ static_cast<decltype( sload)>(a.stencil_load_op) },
		sstore{ static_cast<decltype(sstore)>(a.stencil_store_op) }
	{
	}
	//AttachmentOps(AttachmentOps&&) = default;
	//AttachmentOps& operator=(AttachmentOps&&) = default;
	//AttachmentOps(const AttachmentOps&) = default;
	//AttachmentOps& operator=(const AttachmentOps&) = default;
};
bool operator==(const AttachmentOps& lhs, const AttachmentOps& rhs)
{
	return lhs.load == rhs.load
		&&
		lhs.sload == rhs.sload
		&&
		lhs.store == rhs.store
		&&
		lhs.sstore == rhs.sstore;
}
bool operator==(const idk::small_vector<AttachmentOps>& lhs, const idk::small_vector<AttachmentOps>& rhs)
{
	bool same = lhs.size() == rhs.size();
	if (same)
		for (auto litr = lhs.cbegin(), ritr = rhs.cbegin(); litr != lhs.cend(); ++litr, ++ritr)
		{
			same &= *litr == *ritr;
		}
	return same;
}

struct rp_type_t
{
	uint32_t num_col = 0;
	bool has_depth   = false;
	bool has_stencil = false;
	idk::small_vector<AttachmentOps> attachments;
	bool operator==(const rp_type_t& rhs)const
	{
		return num_col == rhs.num_col && has_depth == rhs.has_depth && has_stencil == rhs.has_stencil && attachments == rhs.attachments;
	}
};

namespace std
{
	template<>
	struct hash<rp_type_t>
	{
		size_t operator()(const rp_type_t& rhs)const
		{
			return static_cast<size_t>(rhs.num_col) << 2 | static_cast<size_t>(rhs.has_depth) << 1 | static_cast<size_t>(rhs.has_stencil);
		}
	};
}

rp_type_t to_rp_type(idk::FrameBuffer& fb)
{
	rp_type_t result;
	result.num_col    = static_cast<uint32_t>(fb.attachments.size());
	result.has_depth  = static_cast<bool    >(fb.depth_attachment  );
	result.has_stencil= static_cast<bool    >(fb.stencil_attachment);
	result.attachments.reserve(result.num_col);
	for (auto& attachment : fb.attachments)
	{
		result.attachments.emplace_back(*attachment);
	}
	return result;
}

namespace idk::vkn
{


	VulkanView& View();

	vk::AttachmentLoadOp(&InitLoadOp())[3]
	{
		static vk::AttachmentLoadOp map[3];
		map[s_cast<uint32_t>(LoadOp::eClear)] = vk::AttachmentLoadOp::eClear;
		map[s_cast<uint32_t>(LoadOp::eDontCare)] = vk::AttachmentLoadOp::eDontCare;
		map[s_cast<uint32_t>(LoadOp::eLoad)] = vk::AttachmentLoadOp::eLoad;
		return map;
	}

	vk::AttachmentLoadOp MapLoadOp(LoadOp load_op)
	{
		static auto& map = InitLoadOp();
		return map[s_cast<uint32_t>(load_op)];
	}
	vk::AttachmentStoreOp(&InitStoreOp())[2]
	{
		static vk::AttachmentStoreOp map[2];
		map[s_cast<uint32_t>(StoreOp::eDontCare)] = vk::AttachmentStoreOp::eDontCare;
		map[s_cast<uint32_t>(StoreOp::eStore)] = vk::AttachmentStoreOp::eStore;
		return map;
	}

	vk::AttachmentStoreOp MapStoreOp(StoreOp Store_op)
	{
		static auto& map = InitStoreOp();
		return map[s_cast<uint32_t>(Store_op)];
	}

	struct VknFrameBufferFactory::Pimpl
	{
		hlp::MemoryAllocator allocator;
		vk::UniqueFence fence;
		hash_table<rp_type_t, RenderPassObj> render_passes;//probably should move this to a manager

		vk::UniqueRenderPass CreateRenderPass(uint32_t num_col,const VknFrameBuffer& fb)
		{
			//uint32_t num_col = fb.attachments.size();
			const vector<unique_ptr<Attachment>>& col_attachments = fb.attachments;
			vector< vk::AttachmentDescription> attachments_desc(num_col);
			vector< vk::AttachmentReference>   attachments_ref;
			vector< vk::AttachmentReference>   in_ref;

			vk::AttachmentDescription depth_attachment_desc;
			vk::AttachmentDescription stencil_attachment_desc;
			vk::AttachmentReference depth_attachment_ref;
			//TODO: Figure out how to deal with DepthStencil since vulkan only accepts them together, they can't be separate.
			vk::AttachmentReference stencil_attachment_ref;

			uint32_t i = 0;
			for (i=0; i<num_col;++i)
			{
				auto& col_attachment = col_attachments[i];
				auto& vk_att = static_cast<const VknAttachment&>(*col_attachment);
				auto& vk_tex = vk_att.buffer.as<VknTexture>();
				if (vk_att.is_input_att)
				{
					in_ref.emplace_back(
						vk::AttachmentReference{
							i
							,(vk_att.override_as_depth)? vk::ImageLayout::eShaderReadOnlyOptimal:vk::ImageLayout::eShaderReadOnlyOptimal
						});
				}
				else
				{
					attachments_ref.emplace_back(
						vk::AttachmentReference{
							i
							,vk::ImageLayout::eColorAttachmentOptimal
						});
				}
				attachments_desc[i] =
				vk::AttachmentDescription{
						vk::AttachmentDescriptionFlags{}
						, vk_tex.format
						, vk::SampleCountFlagBits::e1
						, MapLoadOp(vk_att.load_op)
						, MapStoreOp(vk_att.store_op)
						, MapLoadOp (vk_att.stencil_load_op)
						, MapStoreOp(vk_att.stencil_store_op)
						, (vk_att.load_op==LoadOp::eLoad)? vk::ImageLayout::eGeneral :vk::ImageLayout::eUndefined
						, vk::ImageLayout::eGeneral
				};
			}
			vk::AttachmentReference* pdepth_ref = nullptr;
			//for (auto& depth_attachment : depth_attachments)
			if (fb.depth_attachment)
			{
				auto& vk_att = static_cast<const VknAttachment&>(*fb.depth_attachment);
				auto& vk_tex = vk_att.buffer.as<VknTexture>();
				depth_attachment_ref =
					vk::AttachmentReference{
						i++
						,vk::ImageLayout::eDepthStencilAttachmentOptimal
				};
				depth_attachment_desc =
					vk::AttachmentDescription{
							vk::AttachmentDescriptionFlags{}
							, vk_tex.format
							, vk::SampleCountFlagBits::e1
							, MapLoadOp(vk_att.load_op)
							, MapStoreOp(vk_att.store_op)
							, MapLoadOp(vk_att.stencil_load_op)
							, MapStoreOp(vk_att.stencil_store_op)
							, vk::ImageLayout::eUndefined
							, vk::ImageLayout::eGeneral
				};
				attachments_desc.emplace_back(depth_attachment_desc);
				pdepth_ref = &depth_attachment_ref;
			}

			vk::SubpassDescription subpass
			{
				vk::SubpassDescriptionFlags{}
				,vk::PipelineBindPoint::eGraphics
				,hlp::arr_count(in_ref),(hlp::arr_count(in_ref)) ? std::data(in_ref) : nullptr
				,hlp::arr_count(attachments_ref),(hlp::arr_count(attachments_ref))?std::data(attachments_ref):nullptr
				,nullptr
				,pdepth_ref
			};
			vk::PipelineStageFlags src_mask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
			vk::PipelineStageFlags dest_mask{};
			vk::AccessFlags access_flag = {};
			if (hlp::arr_count(attachments_ref))
			{
				dest_mask |= vk::PipelineStageFlagBits::eColorAttachmentOutput;
				access_flag = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
			}


			if (fb.depth_attachment)
			{
				dest_mask |= vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests;
				access_flag |= vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
			}

			auto dependency = vk::SubpassDependency{
						VK_SUBPASS_EXTERNAL//src
						,0U				   //dest
						,src_mask
						,dest_mask
						,vk::AccessFlags{}
						, access_flag
			};
			vk::RenderPassCreateInfo renderPassInfo
			{
				vk::RenderPassCreateFlags{}
				,hlp::arr_count(attachments_desc),(hlp::arr_count(attachments_desc)) ? std::data(attachments_desc):nullptr
				,1,&subpass
				,1,&dependency
			};
			return View().Device()->createRenderPassUnique(renderPassInfo);
		}
		//Gets or the appropriate renderpass, initalizes if not-present.
		RenderPassObj GetRenderPass(rp_type_t rp_type, const VknFrameBuffer& fb)
		{
			auto itr = render_passes.find(rp_type);
			if (itr == render_passes.end())
			{
				itr = render_passes.emplace(rp_type,
					CreateRenderPass(
						rp_type.num_col,
						fb
					)).first;
			}
			return itr->second;
		}
		Pimpl() : allocator{*View().Device(), View().PDevice()}
		{
			fence = View().Device()->createFenceUnique(vk::FenceCreateInfo{});
		}
	};

	VknFrameBufferFactory::VknFrameBufferFactory(): _pimpl{std::make_unique<Pimpl>()}
	{
	}

	unique_ptr<FrameBuffer> VknFrameBufferFactory::Create()
	{
		return std::make_unique<VknFrameBuffer>();
	}

	unique_ptr<FrameBuffer> VknFrameBufferFactory::GenerateDefaultResource()
	{
		auto fb = std::make_unique<VknFrameBuffer>();
		//TODO actually create a default framebuffer and use it here
		return fb;
	}

	void VknFrameBufferFactory::CreateAttachment(AttachmentType type, const AttachmentInfo& info, ivec2 size, unique_ptr<Attachment>& out)
	{
		using tex_create_ptr_t = TexCreateInfo(*)(uint32_t width, uint32_t height);
		constexpr static tex_create_ptr_t cr8_funcs[]
		{
			&ColorBufferTexInfo,
			&DepthBufferTexInfo
		};
		auto& allocator = _pimpl->allocator;
		auto fence = *_pimpl->fence;
		out =std::make_unique<VknAttachment>();
		out->load_op = info.load_op;
		out->store_op = info.store_op;
		
		auto preset = static_cast<bool>(info.buffer);
		RscHandle<VknTexture> tex = (preset) ? RscHandle<VknTexture>{*info.buffer} : Core::GetResourceManager().Create<VknTexture>();
		auto& vout = static_cast<VknAttachment&>(*out);
		vout.override_as_depth = info.override_as_depth;
		vout.is_input_att = info.is_input_att;
		type = (info.override_as_depth) ? AttachmentType::eDepth: type;
		if(!preset )
		{
			auto& t = *tex;
			TextureLoader loader;
			TextureOptions opt{t.GetMeta()};
			TexCreateInfo tci = cr8_funcs[type](size.x,size.y);
			tci.image_usage |= vk::ImageUsageFlagBits::eTransferSrc| vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eInputAttachment;
			opt.internal_format = info.internal_format;
			opt.filter_mode = info.filter_mode;
			tci.internal_format = MapFormat(opt.internal_format);
			loader.LoadTexture(t, allocator, fence, opt, tci, {});
			t.GetMeta() = opt;
		}
		out->own_buffer = !preset;
		out->buffer = tex;
	}
	void VknFrameBufferFactory::PreReset(FrameBuffer& )
	{
	}
//#pragma optimize("",off)
	void VknFrameBufferFactory::Finalize(FrameBuffer& h_fb, SpecializedInfo* specialized_info)
	{
		auto* spec_info = static_cast<VknSpecializedInfo*>(specialized_info);
		VknFrameBuffer& fb = static_cast<VknFrameBuffer&>(h_fb);
		VulkanView& vknView = View();
		vector<vk::ImageView> image_views(fb.NumAttachments());
		uint32_t i = 0;
		for (auto& col_attachment : fb.attachments)
		{
			auto& attachment = static_cast<VknAttachment&>(*col_attachment);
			auto& v_tex = attachment.buffer.as<VknTexture>();
			image_views[i++] = v_tex.ImageView();
		}
		if (fb.depth_attachment)
		{
			auto& attachment = static_cast<VknAttachment&>(*fb.depth_attachment);
			auto& v_tex = attachment.buffer.as<VknTexture>();
			image_views[i++] = v_tex.ImageView();
		}
		rp_type_t rp_type = to_rp_type(fb);//(s_cast<uint64_t>(i))<<2 | (s_cast<uint32_t>(fb.depth_attachment)<<1) | s_cast<bool>(fb.stencil_attachment);

		vk::FramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.renderPass = *((spec_info)?spec_info->render_pass:_pimpl->GetRenderPass(rp_type,fb));
		framebufferInfo.attachmentCount = hlp::arr_count(image_views);
		framebufferInfo.pAttachments = std::data(image_views);
		framebufferInfo.width  = s_cast<uint32_t>(fb.size.x);
		framebufferInfo.height = s_cast<uint32_t>(fb.size.y);
		framebufferInfo.layers = fb.NumLayers();


		fb.SetFramebuffer(vknView.Device()->createFramebufferUnique(framebufferInfo, nullptr, vknView.Dispatcher()), (spec_info)? spec_info->render_pass:_pimpl->GetRenderPass(rp_type, fb));
	}
}
