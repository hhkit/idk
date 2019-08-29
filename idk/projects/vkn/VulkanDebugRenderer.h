#pragma once
#include <gfx/DebugRenderer.h>
namespace idk::vkn
{
	class VulkanState;

	class VulkanDebugRenderer :
		public DebugRenderer
	{
	public:
		VulkanDebugRenderer();
		~VulkanDebugRenderer() override;

		void Init( const pipeline_config& pipeline_config) override;
		void Shutdown() override;
		void Render(const mat4& view, const mat4& projection) override;
	private:
		struct pimpl;
		VulkanState* vulkan_{};
		std::unique_ptr<pimpl> impl{};
	};

}

