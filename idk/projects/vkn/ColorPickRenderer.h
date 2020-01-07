#pragma once
#include <gfx/ColorPickRequest.h>
#include <vkn/GraphicsState.h>
#include <vkn/RenderStateV2.h>
namespace idk::vkn
{

	struct ColorPickRenderer
	{
		void Render(vector<ColorPickRequest>& requests, SharedGraphicsState& shared_gs, RenderStateV2& rs);
		ColorPickRenderer();
		~ColorPickRenderer();
	private:
		struct PImpl;
		std::unique_ptr<PImpl> _pimpl;
	};
}