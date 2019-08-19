#pragma once
#include <gfx/DebugRenderer.h>
namespace idk
{
	class Vulkan;

	class VulkanDebugRenderer :
		public DebugRenderer
	{
	public:
		VulkanDebugRenderer(Vulkan& vulkan);
		void Init( const idk::pipeline_config& pipeline_config, const idk::uniform_info& uniform_info) override;
		void Render() override;
		~VulkanDebugRenderer() override;
	private:
		struct pimpl;
		Vulkan* vulkan_{};
		std::unique_ptr<pimpl> impl{};
	};

}

