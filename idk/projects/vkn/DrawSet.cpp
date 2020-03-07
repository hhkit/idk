#include "pch.h"
#include "DrawSet.h"
#include <gfx/GraphicsSystem.h>
#include <vkn/VulkanMesh.h>
#include <res/ResourceHandle.inl>
#include <ds/span.inl>
#include <vkn/RenderUtil.h>

#include <vkn/vector_buffer.h>
namespace idk::vkn
{
//#pragma optimize("",off)
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
			the_interface.DrawIndexed(mesh.IndexCount(), static_cast<uint32_t>(ro.num_instances), 0, 0, static_cast<uint32_t>(ro.instanced_index));
		return result;
	}
	void InstMeshDrawSet::Render(RenderInterface& the_interface, bindings::RenderBindings& binders)
	{
		
		std::array description{
			buffer_desc
		{
			buffer_desc::binding_info{ std::nullopt,sizeof(FakeMat4<float>) * 2,VertexRate::eInstance },
		{ buffer_desc::attribute_info{ AttribFormat::eMat4,4,0,true },
		buffer_desc::attribute_info{ AttribFormat::eMat4,8,sizeof(FakeMat4<float>),true }
		}
		}
		};
		{
			binders.Bind(the_interface);
			the_interface.SetBufferDescriptions(description);
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
						auto& req = *dc.renderer_req;
						the_interface.BindVertexBuffer(req.instanced_requirements.at(vtx::InstAttrib::ModelTransform), _inst_mesh_render_buffer, 0);
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

		the_interface.SetBufferDescriptions({});
		binders.Bind(the_interface);
		for (auto& ptr_dc : _draw_calls)
		{
			auto& dc = *ptr_dc;
			[[maybe_unused]] auto& mat_inst = *dc.material_instance; //Debug only
			if (!binders.Skip(the_interface, dc))
			{
				binders.Bind(the_interface, dc);
				DrawMeshBuffers(the_interface, dc);
			}
		}//End of draw_call loop
	}
//#pragma optimize("",off)
	
	renderer_attributes FsqDrawSet::_req = { {
			std::make_pair(vtx::Attrib::Position, 0),
			std::make_pair(vtx::Attrib::Normal, 1),
			std::make_pair(vtx::Attrib::UV, 2) }
	};
	FsqDrawSet::FsqDrawSet(MeshType::_enum mesh_type, bool draw_till_skip) :FsqDrawSet{ draw_till_skip }
	{
		_mesh_type = mesh_type;
		_fsq_ro.mesh = Mesh::defaults[_mesh_type];
	}
	FsqDrawSet::FsqDrawSet(bool draw_till_skip) : _draw_till_skip{draw_till_skip}
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
		return rendering && _draw_till_skip;
	}
	PerLightDrawSet::PerLightDrawSet(bool draw_till_skip ) : FsqDrawSet{draw_till_skip}
	{
}
	TextMeshDrawSet::TextMeshDrawSet(span<const FontRenderData> fonts, span<const hlp::vector_buffer> buffers):_texts{fonts}, _text_buffers{buffers}
	{
	}
	void TextMeshDrawSet::Render(RenderInterface& the_interface, bindings::RenderBindings& bindings)
	{
		size_t i = 0;
		bindings.Bind(the_interface);
		for (auto& text : _texts)
		{
			bindings.BindFont(the_interface,text);
			//Bind Attribute
			the_interface.BindVertexBuffer(0, _text_buffers[i].buffer(), 0);

			//Finalize draw call
			the_interface.Draw(s_cast<uint32_t>(text.coords.size()), 1,0,0 );
			//the_interface.FinalizeDrawCall(font_ro_inst.emplace_back(std::move(RenderObject{ font_ro })));
			++i;
		}
	}
//#pragma optimize("",off)
	void PerLightDrawSet::Render(RenderInterface& the_interface, bindings::RenderBindings& bindings)
	{
		bindings.Bind(the_interface);
		while (BindRo(the_interface, bindings));
	}
	ParticleDrawSet::ParticleDrawSet(span<const ParticleRange> particles, const hlp::vector_buffer& buffer) : _particles{ particles }, _particle_buffer{buffer}
	{
	}
	void ParticleDrawSet::Render(RenderInterface& the_interface, bindings::RenderBindings& bindings)
	{
		static const renderer_attributes particle_vertex_req = renderer_attributes{ {
								{ vtx::Attrib::Position, 0 },
								{ vtx::Attrib::UV, 1 },
							}
		};;

		static const vector<buffer_desc> particle_buffer_desc
		{ buffer_desc
		{
					   buffer_desc::binding_info{ {},sizeof(ParticleObj),VertexRate::eInstance },
							   {
								   buffer_desc::attribute_info
				   {
					   AttribFormat::eSVec3,2,offsetof(ParticleObj,ParticleObj::position),true
				   },
				   buffer_desc::attribute_info
				   {
					   AttribFormat::eSVec1,3,offsetof(ParticleObj,ParticleObj::rotation),true
				   },
				   buffer_desc::attribute_info
				   {
					   AttribFormat::eSVec1,4,offsetof(ParticleObj,ParticleObj::size),true
				   },
				   buffer_desc::attribute_info
				   {
					   AttribFormat::eSVec4,5,offsetof(ParticleObj,ParticleObj::color),true
				   },
							   }
		}
		};
		bindings.Bind(the_interface);
		for (auto& elem : _particles)
		{
			InstRenderObjects part_ro;

			const auto material = elem.material_instance->material;
			part_ro.material_instance = elem.material_instance;
			part_ro.mesh = Mesh::defaults[MeshType::FSQ];
			part_ro.renderer_req = &particle_vertex_req;
			the_interface.SetBufferDescriptions(particle_buffer_desc);
			//TODO bind materials
			if (!bindings.Skip(the_interface, part_ro))
			{
				bindings.Bind(the_interface, part_ro);
				part_ro.num_instances = elem.num_elems;
				part_ro.instanced_index = elem.elem_offset;
				the_interface.BindVertexBuffer(2, _particle_buffer.buffer(), 0);
				DrawMeshBuffers(the_interface, part_ro);
			}
		}
	}
}