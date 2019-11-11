#pragma once
#include <gfx/pipeline_config.h>
#include <gfx/uniform_info.h>
#include <gfx/debug_vtx_layout.h>
#include <vkn/GraphicsState.h>
#include <gfx/Mesh.h>
namespace idk
{
	struct ray;
}
namespace idk::vkn
{
	enum DbgShape
	{
		eziPreBegin,
		eCube,
		eSquare,
		eEqTriangle,
		eLine,
		eTetrahedron,
		eSphere,
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
		idk::hash_table<RscHandle<Mesh>, idk::vector<inst_data>> render_info2;
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
		void DrawShape(MeshType shape, const mat4& tfm, const color& color);
		void Draw(const ray& ray, const color& color);
		void Draw(const sphere& sphere, const color& color);
		void Draw(const box& box, const color& color);
		void Draw(const aabb& o_box, const color& color);
		void Render(const mat4& view, const mat4& projection, GraphicsState& out);

		const vector<DbgDrawCall>& DbgDrawCalls()const;
		const VulkanPipeline& GetPipeline()const;
		void GrabDebugBuffer();
	private:
		struct pimpl;
		VulkanState* vulkan_{};
		std::unique_ptr<pimpl> impl{};
		std::shared_ptr<debug_info> info;
	};

}

