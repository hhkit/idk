#pragma once
#include <gfx/DebugRenderer.h>
namespace idk::vkn
{
	class VulkanState;

	class VulkanDebugRenderer :
		public DebugRenderer
	{
	public:
		VulkanDebugRenderer(VulkanState& vulkan);
		~VulkanDebugRenderer() override;

		void Init( const pipeline_config& pipeline_config, const uniform_info& uniform_info) override;
		void Shutdown() override;
		void Render() override;
	private:
		struct pimpl;
		VulkanState* vulkan_{};
		std::unique_ptr<pimpl> impl{};
	};

}

