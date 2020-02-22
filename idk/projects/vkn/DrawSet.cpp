#include "pch.h"
#include "DrawSet.h"
#include <gfx/GraphicsSystem.h>
#include <vkn/VulkanMesh.h>
#include <res/ResourceHandle.inl>
#include <ds/span.inl>
namespace idk::vkn
{
#pragma optimize("",off)
	InstMeshDrawSet::InstMeshDrawSet(span<const InstRenderObjects> inst_draw_range, vk::Buffer inst_mesh_render_buffer) : 
		_inst_draw_range{ inst_draw_range },
		_inst_mesh_render_buffer{ inst_mesh_render_buffer }
	{
	}
		
	bool BindMeshBuffers(RenderInterface& the_interface,const VulkanMesh& mesh, const renderer_attributes& attribs)
	{
		for (auto&& [attrib, location] : attribs.mesh_requirements)
		{
			if (!mesh.Has(attrib))
				return false;
			auto& attrib_buffer = mesh.Get(attrib);
			the_interface.BindVertexBuffer(location, *attrib_buffer.buffer(), attrib_buffer.offset);
		}
		auto& vmesh = mesh;
		auto& idx_buffer = vmesh.GetIndexBuffer();
		if (idx_buffer && idx_buffer->buffer())
		{
			the_interface.BindIndexBuffer(*idx_buffer->buffer(), idx_buffer->offset, vmesh.IndexType());
		};
		return true;
	}

	bool BindMeshBuffers(RenderInterface& the_interface, const RenderObject& ro)
	{
		return BindMeshBuffers(the_interface, ro.mesh.as<VulkanMesh>(), *ro.renderer_req);
	}
	bool DrawMeshBuffers(RenderInterface& the_interface, const RenderObject& ro)
	{
		auto& mesh = ro.mesh.as<VulkanMesh>();
		bool result = false;
		if(result|=BindMeshBuffers(the_interface, mesh, *ro.renderer_req))
			the_interface.DrawIndexed(mesh.IndexCount(), 1, 0, 0, 0);
		return result;
	}
	bool DrawMeshBuffers(RenderInterface& the_interface, const InstRenderObjects& ro)
	{
		auto& mesh = ro.mesh.as<VulkanMesh>();
		bool result = false;
		if(result |= BindMeshBuffers(the_interface, ro))
			the_interface.DrawIndexed(mesh.IndexCount(), ro.num_instances, 0, 0, ro.instanced_index);
		return result;
	}
	void InstMeshDrawSet::Render(RenderInterface& the_interface, bindings::RenderBindings& binders)
	{
		span<const AnimatedRenderObject> draw_calls;
		{
			binders.Bind(the_interface);
			{
				//auto range_opt = state.range;
				//if (!range_opt)
				//	range_opt = GraphicsSystem::RenderRange{ CameraData{},0,state.inst_ro->size() };

				//auto& inst_draw_range = *range_opt;
				for (auto& dc : _inst_draw_range)
				{
					auto& mat_inst = *dc.material_instance;
					if (mat_inst.material && !binders.Skip(the_interface, dc))
					{
						binders.Bind(the_interface, dc);
						auto& mesh = dc.mesh.as<VulkanMesh>();
						the_interface.BindVertexBuffer(4, _inst_mesh_render_buffer, 0);
						//BindMeshBuffers(the_interface, mesh, *dc.renderer_req);
						//the_interface.DrawIndexed(mesh.IndexCount(), dc.num_instances, 0, 0, dc.instanced_index);
						DrawMeshBuffers(the_interface, dc);
					}
				}

			}
		}


	}
	SkinnedMeshDrawSet::SkinnedMeshDrawSet(span<const AnimatedRenderObject* const> draw_calls):_draw_calls{draw_calls}
	{
		_draw_calls = _draw_calls;
	}
	void SkinnedMeshDrawSet::Render(RenderInterface& the_interface, bindings::RenderBindings& binders)
	{

		binders.Bind(the_interface);
		for (auto& ptr_dc : _draw_calls)
		{
			auto& dc = *ptr_dc;
			auto& mat_inst = *dc.material_instance;
			if (!binders.Skip(the_interface, dc))
			{
				binders.Bind(the_interface, dc);
				DrawMeshBuffers(the_interface, dc);
			}
		}//End of draw_call loop
	}
#pragma optimize("",off)
	
	renderer_attributes FsqDrawSet::_req = { {
			std::make_pair(vtx::Attrib::Position, 0),
			std::make_pair(vtx::Attrib::Normal, 1),
			std::make_pair(vtx::Attrib::UV, 2) }
	};
	FsqDrawSet::FsqDrawSet(MeshType mesh_type):_mesh_type{mesh_type}
	{
		_fsq_ro.mesh = Mesh::defaults[_mesh_type];
		_fsq_ro.renderer_req = &_req;
	}
	void FsqDrawSet::Render(RenderInterface& the_interface, bindings::RenderBindings& bindings)
	{
		bindings.Bind(the_interface);
		BindRo(the_interface, bindings);
		
	}
	bool FsqDrawSet::BindRo(RenderInterface& the_interface, bindings::RenderBindings& bindings)
	{
		auto& fsq_ro = _fsq_ro;
		bool rendering = !bindings.Skip(the_interface, fsq_ro);
		if (rendering)
		{
			bindings.Bind(the_interface, fsq_ro);
			DrawMeshBuffers(the_interface, fsq_ro);
		};
		return rendering;
	}
	PerLightDrawSet::PerLightDrawSet()
	{
	}
	void PerLightDrawSet::Render(RenderInterface& the_interface, bindings::RenderBindings& bindings)
	{
		bindings.Bind(the_interface);
		while (BindRo(the_interface, bindings));
	}
}