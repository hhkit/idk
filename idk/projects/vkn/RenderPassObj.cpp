#include "pch.h"
#include "RenderPassObj.h"

namespace idk::vkn
{
	void RenderPassObj::reset(vk::RenderPass const& value)
	{
		if (!ctrl_blk && !value)
			return;
		ctrl_blk.reset();
		//ctrl_blk->my_rp.reset(value);
		//ctrl_blk.reset();
	}

	vk::RenderPass RenderPassObj::release()
	{
		auto result = (ctrl_blk) ? ctrl_blk->my_rp.release() : vk::RenderPass{};
		ctrl_blk.reset();
		return result;
	}

	void RenderPassObj::swap(RenderPassObj& rhs)
	{
		ctrl_blk.swap(rhs.ctrl_blk);
	}

}