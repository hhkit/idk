#pragma once
#include <vulkan/vulkan.hpp>
namespace idk::vkn
{

	struct RenderPassObj
	{
		RenderPassObj() {}
		RenderPassObj(vk::UniqueRenderPass&& rp) :ctrl_blk{ std::make_unique<Internal>(Internal{std::move(rp)}) } {}
		RenderPassObj& operator=(vk::UniqueRenderPass&& rp)
		{
			if (!ctrl_blk)
				ctrl_blk = std::make_unique<Internal>();
			ctrl_blk->my_rp = std::move(rp);
			return *this;
		}
		RenderPassObj& operator=(const RenderPassObj& rp)
		{
			ctrl_blk = rp.ctrl_blk;
			return *this;
		}
		operator bool()const noexcept
		{
			return (ctrl_blk && ctrl_blk->my_rp);
		}
		vk::UniqueRenderPass& operator->()
		{
			return ctrl_blk->my_rp;
		}
		operator vk::RenderPass()
		{
			return *ctrl_blk->my_rp;
		}
		vk::RenderPass& operator*()
		{
			return *ctrl_blk->my_rp;
		}
		const vk::UniqueRenderPass& operator->()const
		{
			return ctrl_blk->my_rp;
		}
		const vk::RenderPass& operator*()const
		{
			return *ctrl_blk->my_rp;
		}

		void reset(vk::RenderPass const& value = vk::RenderPass());

		vk::RenderPass release();

		void swap(RenderPassObj& rhs);
		
	private:
		struct Internal
		{
			vk::UniqueRenderPass my_rp;
		};
		std::shared_ptr<Internal> ctrl_blk;

	};
}