#pragma once
#include <vkn/BaseRenderPass.h>
#include <vkn/DrawSetRenderPass.h>
#include <vkn/renderpasses/SimpleRenderTargetPass.h>

#include <vkn/DrawSet.h>
#include <vkn/ParticleBindings.h>

#include <vkn/FrameGraph.h>

namespace idk::vkn::renderpasses
{

	
	class ParticleRenderer
	{
	public:
		void Init();

		std::pair<FrameGraphResource, FrameGraphResource> AddPass(FrameGraph& grpah, const GraphicsState& state,FrameGraphResource color, FrameGraphResource depth);

	private:
		
	};
}