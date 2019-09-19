#pragma once
#include <gfx/pipeline_config.h>
#include <gfx/uniform_info.h>
#include <gfx/debug_vtx_layout.h>

namespace idk::vkn
{
	enum DbgShape
	{
		eziPreBegin,
		eCube,
		eSquare,
		eEqTriangle,
		eziCount
	};
	struct debug_info
	{
		struct inst_data
		{
			color col;
			mat4 transform;
		};
		idk::hash_table<DbgShape, idk::vector<inst_data>> render_info;
	};

	class VulkanState;

	class VulkanDebugRenderer
	{
	public:
		VulkanDebugRenderer();
		~VulkanDebugRenderer();

		void Init();
		void Init(const pipeline_config& pipeline_config);
		void Shutdown();
		void DrawShape(DbgShape shape, const mat4& tfm, const color& color);
		void Render(const mat4& view, const mat4& projection);

		void GrabDebugBuffer();
	private:
		struct pimpl;
		VulkanState* vulkan_{};
		std::unique_ptr<pimpl> impl{};
		std::shared_ptr<debug_info> info;
	};

}

