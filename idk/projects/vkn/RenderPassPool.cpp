#include "pch.h"
#include "RenderPassPool.h"

#include <vkn/VulkanView.h>

#include <vkn/BufferHelpers.inl>

#include <ds/span.inl>

namespace idk::vkn
{
	struct Derp
	{
		uint32_t num_subpasses;
		uint32_t num_attachments;
		uint32_t num_dependencies;
	};


	RenderPassPool::rp_type_t RenderPassPool::ComputeType(const rp_info_t& rpci) const
	{
		rp_type_t rp;
		rp.reserve(256);

		rp.append(hlp::to_data(rpci.attachmentCount));
		rp.append(hlp::to_data(rpci.subpassCount));
		rp.append(hlp::to_data(rpci.dependencyCount));
		
		for (size_t i = 0; i < rpci.attachmentCount; ++i)
		{
			rp.append(hlp::to_data(rpci.pAttachments[i]));
		}
		for (size_t i = 0; i < rpci.subpassCount; ++i)
		{
			rp.append(hlp::to_data(rpci.pSubpasses[i].colorAttachmentCount));
			rp.append(hlp::to_data(rpci.pSubpasses[i].inputAttachmentCount));
			rp.append(hlp::to_data(rpci.pSubpasses[i].preserveAttachmentCount));

			{
				span sp = { rpci.pSubpasses[i].pColorAttachments,rpci.pSubpasses[i].pColorAttachments + rpci.pSubpasses[i].colorAttachmentCount };
				for (auto ca : sp)
				{
					rp.append(hlp::to_data(ca));
				}
			}
			{
				span sp = { rpci.pSubpasses[i].pInputAttachments,rpci.pSubpasses[i].pInputAttachments + rpci.pSubpasses[i].inputAttachmentCount };
				for (auto ca : sp)
				{
					rp.append(hlp::to_data(ca));
				}
			}
			{
				span sp = { rpci.pSubpasses[i].pPreserveAttachments,rpci.pSubpasses[i].pPreserveAttachments + rpci.pSubpasses[i].preserveAttachmentCount };
				for (auto ca : sp)
				{
					rp.append(hlp::to_data(ca));
				}
			}
		}
		for (size_t i = 0; i < rpci.dependencyCount; ++i)
		{
			rp.append(hlp::to_data(rpci.pDependencies[i]));
		}
		return rp;
	}
	RenderPassObj RenderPassPool::CreateRenderPass(rp_type_t type, const rp_info_t& rpci)
	{
		auto urp= View().Device()->createRenderPassUnique(rpci);
		auto rp = *urp;
		return _subpools.emplace(type, std::move(urp)).first->second;
		;
	}

	FramebufferPool::fb_type_t FramebufferPool::ComputeType(const fb_info_t& fbci) const
	{
		fb_type_t fb;
		fb.reserve(256);

		//Might be too naive

		fb.append(hlp::to_data(fbci.attachmentCount));
		fb.append(hlp::to_data(fbci.flags));
		fb.append(hlp::to_data(fbci.height));
		fb.append(hlp::to_data(fbci.width));
		fb.append(hlp::to_data(fbci.layers));
		fb.append(hlp::to_data(fbci.renderPass));

		for (size_t i = 0; i < fbci.attachmentCount; ++i)
		{
			fb.append(hlp::to_data(fbci.pAttachments[i]));
		}
		return fb;
	}

	vk::Framebuffer FramebufferPool::CreateFramebuffer(fb_type_t type, const fb_info_t& fbci)
	{
		auto ufb = View().Device()->createFramebufferUnique(fbci);
		auto fb = *ufb;
		_subpools.emplace(type, std::move(ufb));
		return fb;
	}

}