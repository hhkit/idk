#pragma once
#include <gfx/DebugRenderer.h>

namespace idk
{
	class VulkanDebugRenderer :
		public DebugRenderer
	{
	public:
		void Init(GfxSystem& system, const idk::pipeline_config& pipeline_config, const idk::uniform_info& uniform_info) override;
		void Render() override;
	private:
		struct pimpl;

		std::unique_ptr<pimpl> impl;
	};

}

