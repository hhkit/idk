#pragma once
#include <gfx/ColorPickRequest.h>
#include <vkn/GraphicsState.h>
#include <vkn/RenderStateV2.h>
namespace idk::vkn
{

	struct ColorPickRenderer
	{
		void PreRender(vector<ColorPickRequest>& requests, const SharedGraphicsState& shared_gs, size_t total_num_insts, vk::CommandBuffer cmd_buffer);
		void Render(vector<ColorPickRequest>& requests, const SharedGraphicsState& shared_gs, RenderStateV2& rs);
		ColorPickRenderer();
		~ColorPickRenderer();
	private:
		struct PImpl;
		std::unique_ptr<PImpl> _pimpl;
	};
}