#include "pch.h"

#include <res/ResourceHandle.inl>

#include <vkn/VknTextureLoader.h>

#include <vkn/VulkanTextureFactory.h>
#include <vkn/VknTextureView.h>
#include <vkn/VulkanView.h>
#include <vkn/VulkanState.h>
#include <res/ResourceMeta.inl>
namespace idk::vkn
{
	void ResizeTex(uvec2 sz, RscHandle<Texture> tx) 
	{
		auto tex = RscHandle<VknTexture>{ tx };
		TextureLoader loader;
		auto& factory = Core::GetResourceManager().GetFactory<VulkanTextureFactory>();

		TextureOptions to{tex->GetMeta()};
		TexCreateInfo tci{ sz.x,sz.y,tex->format,tex->usage};
		tci.aspect = vk::ImageAspectFlagBits::eColor;
		tci.layout = vk::ImageLayout::eTransferDstOptimal;
		
		loader.LoadTexture(*tex, factory.GetAllocator(), factory.GetFence(), to, tci, {}, tex.guid);
	}
	void CopyTempTex(VknTextureView src, VknTextureView target, vk::CommandBuffer cmd_buffer)
	{
		auto aspect = vk::ImageAspectFlagBits::eColor;
		//Transit RTD -> Transfer
		vk::ImageMemoryBarrier depth
		{
			{},
			vk::AccessFlagBits::eTransferWrite,
			vk::ImageLayout::eUndefined,
			vk::ImageLayout::eTransferDstOptimal,
			*View().QueueFamily().graphics_family,
			*View().QueueFamily().graphics_family,
			target.Image(),
			vk::ImageSubresourceRange
			{
				aspect,
				0,1,0,1
			}
		};
		std::array<vk::ImageMemoryBarrier, 2> convert_barriers = { depth,depth };
		convert_barriers[1].setDstAccessMask(vk::AccessFlagBits::eTransferRead);
		convert_barriers[1].setOldLayout(vk::ImageLayout::eGeneral);
		convert_barriers[1].setNewLayout(vk::ImageLayout::eTransferSrcOptimal);
		convert_barriers[1].setImage(src.Image());
		cmd_buffer.pipelineBarrier(vk::PipelineStageFlagBits::eBottomOfPipe, vk::PipelineStageFlagBits::eTransfer, vk::DependencyFlagBits::eByRegion, nullptr, nullptr, convert_barriers);

		//Blit
		//expect depth attachments to be the exact same size and format.
		cmd_buffer.copyImage(
			src.Image(), vk::ImageLayout::eTransferSrcOptimal,
			target.Image(), vk::ImageLayout::eTransferDstOptimal,
			vk::ImageCopy
			{
				vk::ImageSubresourceLayers
				{
					aspect,
					0u,0u,1u
				},
				vk::Offset3D{0,0,0},
				vk::ImageSubresourceLayers
				{
					aspect,
					0ui32,0ui32,1ui32
				},
				vk::Offset3D{0,0,0},
				vk::Extent3D{src.Size().x,src.Size().y,1}
			}
		);

		//Transit RTD -> General
		std::array<vk::ImageMemoryBarrier, 2>& return_barriers = convert_barriers;// { depth, depth };
		std::swap(return_barriers[0].dstAccessMask, return_barriers[0].srcAccessMask);
		std::swap(return_barriers[0].oldLayout, return_barriers[0].newLayout);
		return_barriers[0].setDstAccessMask({});// vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite);
		return_barriers[0].setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

		//Transit GD  -> DepthAttachmentOptimal
		return_barriers[1].setSrcAccessMask(vk::AccessFlagBits::eTransferRead);
		return_barriers[1].setDstAccessMask({});
		return_barriers[1].setOldLayout(vk::ImageLayout::eTransferSrcOptimal);
		return_barriers[1].setNewLayout(vk::ImageLayout::eGeneral);
		return_barriers[1].setImage(src.Image());



		cmd_buffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eEarlyFragmentTests, vk::DependencyFlagBits::eByRegion, nullptr, nullptr, return_barriers);

		
	}

}