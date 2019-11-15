#pragma once
#include <gfx/pipeline_config.h>
#include <gfx/uniform_info.h>
#include <gfx/debug_vtx_layout.h>
#include <vkn/GraphicsState.h>
#include <gfx/Mesh.h>

#include <vkn/utils/utils.h>
namespace idk
{
	struct ray;
}
namespace idk::vkn
{
	enum DbgShape
	{
		eCube,
		eSquare,
		eEqTriangle,
		eLine,
		eTetrahedron,
		eSphere
	};
	namespace EnumInfo
	{

		using DbgShapePack = meta::enum_pack<DbgShape,
			DbgShape::eCube,
			DbgShape::eSquare,
			DbgShape::eEqTriangle,
			DbgShape::eLine,
			DbgShape::eTetrahedron,
			DbgShape::eSphere
		>;
		using DbgShapeI = meta::enum_info < DbgShape, DbgShapePack>;

	}
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

		const DbgDrawCall (&DbgDrawCalls()const)[EnumInfo::DbgShapeI::size()];
		const VulkanPipeline& GetPipeline()const;
		void GrabDebugBuffer();
	private:
		struct pimpl;
		VulkanState* vulkan_{};
		std::unique_ptr<pimpl> impl{};
		std::shared_ptr<debug_info> info;
	};

}

