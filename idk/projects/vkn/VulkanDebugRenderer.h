#pragma once
#include <gfx/DebugRenderer.h>
namespace idk
{
	class VulkanState;

	class VulkanDebugRenderer :
		public DebugRenderer
	{
	public:
		VulkanDebugRenderer(VulkanState& vulkan);
		void Init( const pipeline_config& pipeline_config, const uniform_info& uniform_info) override;
		void Render() override;
		~VulkanDebugRenderer() override;
	private:
		struct pimpl;
		VulkanState* vulkan_{};
		std::unique_ptr<pimpl> impl{};
	};

}

