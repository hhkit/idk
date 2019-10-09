#include "pch.h"
#include "VknFramebufferFactory.h"
#include "vkn/VknFrameBuffer.h"
#include <vkn/VknTexture.h>
#include <vkn/VknTextureLoader.h>
#include <vkn/VulkanView.h>
#include <vkn/BufferHelpers.h>
namespace idk::vkn
{

	//void AttTypeToTexOpt(AttachmentType type, TextureOptions& opt)
	//{
	//	switch (type)
	//	{
	//	case idk::eColor:
	//		break;
	//	case idk::eDepth:
	//		break;
	//	case idk::eSizeAT:
	//		break;
	//	default:
	//		break;
	//	}
	//}
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
		hash_table<uint64_t, vk::UniqueRenderPass> render_passes;//probably should move this to a manager

		vk::UniqueRenderPass CreateRenderPass(uint32_t num_col,uint32_t num_depth, const vector<unique_ptr<Attachment>>& col_attachments, const vector<unique_ptr<Attachment>>& depth_attachments)
		{
			vector< vk::AttachmentDescription> attachments_desc(col_attachments.size() + depth_attachments.size());
			vector< vk::AttachmentReference>   attachments_ref (col_attachments.size()+depth_attachments.size());
			uint32_t i = 0;
			for (auto& col_attachment : col_attachments)
			{
				auto& vk_att = static_cast<const VknAttachment&>(*col_attachment);
				auto& vk_tex = vk_att.buffer.as<VknTexture>();
				attachments_ref[i] =
				vk::AttachmentReference{
					i
					,vk::ImageLayout::eColorAttachmentOptimal
				};
				attachments_desc[i++] =
				vk::AttachmentDescription{
						vk::AttachmentDescriptionFlags{}
						, vk_tex.format
						, vk::SampleCountFlagBits::e1
						, MapLoadOp(vk_att.load_op)
						, MapStoreOp(vk_att.store_op)
						, MapLoadOp (vk_att.stencil_load_op)
						, MapStoreOp(vk_att.stencil_store_op)
						, vk::ImageLayout::eUndefined
						, vk::ImageLayout::eGeneral
				};
			}
			for (auto& depth_attachment : depth_attachments)
			{
				auto& vk_att = static_cast<const VknAttachment&>(*depth_attachment);
				auto& vk_tex = vk_att.buffer.as<VknTexture>();
				attachments_ref[i] =
					vk::AttachmentReference{
						i
						,vk::ImageLayout::eDepthStencilAttachmentOptimal
				};
				attachments_desc[i++] =
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
			}
			vk::SubpassDescription subpass
			{
				vk::SubpassDescriptionFlags{}
				,vk::PipelineBindPoint::eGraphics
				,0,nullptr
				,hlp::arr_count(col_attachments),(hlp::arr_count(col_attachments))?std::data(attachments_ref):nullptr
				,nullptr
				,(hlp::arr_count(depth_attachments))?std::data(attachments_ref)+ hlp::arr_count(col_attachments) :nullptr
			};
			vk::PipelineStageFlags src_mask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
			vk::PipelineStageFlags dest_mask{};
			vk::AccessFlags access_flag = {};
			if (hlp::arr_count(col_attachments))
			{
				dest_mask |= vk::PipelineStageFlagBits::eColorAttachmentOutput;
				access_flag = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
			}


			if (hlp::arr_count(depth_attachments))
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
				,hlp::arr_count(attachments_desc),std::data(attachments_desc)
				,1,&subpass
				,1,&dependency
			};
			return View().Device()->createRenderPassUnique(renderPassInfo);
		}

		vk::RenderPass GetRenderPass(uint64_t rp_type, const vector<unique_ptr<Attachment>>& col_attachments, const vector<unique_ptr<Attachment>>& depth_attachments)
		{
			auto itr = render_passes.find(rp_type);
			if (itr == render_passes.end())
			{
				itr = render_passes.emplace(rp_type,
					CreateRenderPass(
						s_cast<uint32_t>(rp_type & 0xFFFFFFFFFF), 
						s_cast<uint32_t>(rp_type >> 32),
						col_attachments,
						depth_attachments
					)).first;
			}
			return *itr->second;
		}
		Pimpl() : allocator{*View().Device(), View().PDevice()}
		{

		}
	};

	VknFrameBufferFactory::VknFrameBufferFactory(): _pimpl{std::make_unique<Pimpl>()}
	{
	}

	void VknFrameBufferFactory::CreateAttachment(const AttachmentInfo& info, ivec2 size, unique_ptr<Attachment>& out)
	{
		using tex_create_ptr_t = TexCreateInfo(*)(uint32_t width, uint32_t height);
		constexpr static tex_create_ptr_t cr8_funcs[]
		{
			&ColorBufferTexInfo,
			&DepthBufferTexInfo
		};
		auto& allocator = _pimpl->allocator;
		auto fence = *_pimpl->fence;
		auto ptr =std::make_unique<VknAttachment>();
		out->load_op = info.load_op;
		out->store_op = info.store_op;

		RscHandle<VknTexture> tex = Core::GetResourceManager().Create<VknTexture>();
		TextureLoader loader;
		TextureOptions opt;
		TexCreateInfo tci = cr8_funcs[info.type](size.x,size.y);
		opt.internal_format = info.internal_format;
		opt.filter_mode = info.filter_mode;
		loader.LoadTexture(*tex, allocator, fence, opt, tci, {});
		out = std::move(ptr);
	}
	void VknFrameBufferFactory::PreReset(FrameBuffer& framebuffer)
	{
	}
	void VknFrameBufferFactory::Finalize(FrameBuffer& h_fb)
	{
		VknFrameBuffer& fb = static_cast<VknFrameBuffer&>(h_fb);
		VulkanView& vknView = View();
		vector<vk::ImageView> image_views(fb.NumAttachments());
		
		uint32_t i = 0;
		for (auto& col_attachment : fb.attachments[AttachmentType::eColor])
		{
			auto& attachment = static_cast<VknAttachment&>(*col_attachment);
			auto& v_tex = attachment.buffer.as<VknTexture>();
			image_views[i++] = v_tex.ImageView();
		}
		uint64_t rp_type = i ;
		for (auto& depth_attachment : fb.attachments[AttachmentType::eDepth])
		{
			auto& attachment = static_cast<VknAttachment&>(*depth_attachment);
			auto& v_tex = attachment.buffer.as<VknTexture>();
			image_views[i++] = v_tex.ImageView();
		}
		rp_type |= (i-rp_type)<<32;

		vk::FramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.renderPass = _pimpl->GetRenderPass(rp_type,fb.attachments[AttachmentType::eColor], fb.attachments[AttachmentType::eDepth]);
		framebufferInfo.attachmentCount = hlp::arr_count(image_views);
		framebufferInfo.pAttachments = std::data(image_views);
		framebufferInfo.width = s_cast<uint32_t> (fb.size.x);
		framebufferInfo.height = s_cast<uint32_t>(fb.size.y);
		framebufferInfo.layers = 1;

		fb.SetFramebuffer(vknView.Device()->createFramebufferUnique(framebufferInfo, nullptr, vknView.Dispatcher()));
	}
}
