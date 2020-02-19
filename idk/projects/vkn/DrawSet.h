#pragma once
#include <vkn/RenderBindings.h>
#include <gfx/Mesh.h>
namespace idk::vkn
{
	template<typename T>
	using binding_only = std::enable_if_t<std::is_base_of_v<bindings::RenderBindings, T>>;
	class BaseDrawSet
	{
	public:
		virtual void Render(RenderInterface& the_interface)=0;
	};
	class BaseDrawLogic
	{
	public:
		virtual void Render(RenderInterface& the_interface,bindings::RenderBindings& bindings) = 0;
	};

	template<typename Binding, typename = binding_only<Binding>>
	class DrawSet : public BaseDrawSet
	{
	public:
		DrawSet(Binding&& binding = Binding{});
	protected:
		Binding&       GetBinding()     { return _binding; }
		const Binding& GetBinding()const{ return _binding; }
	private:
		Binding _binding;
	};
	template<typename Binding,typename DrawLogic>
	class GenericDrawSet : public DrawSet<Binding>
	{
	public:
		using Base = DrawSet<Binding>;
		GenericDrawSet(Binding = {}, DrawLogic = {});
		GenericDrawSet(DrawLogic);

		void Render(RenderInterface& the_interface)override
		{
			_logic.Render(the_interface, this->GetBinding());
		}
	private:
		DrawLogic _logic;
	};

	template<typename ... GenericDrawSets>
	class CombinedMeshDrawSet;



	template<typename ... GenericDrawSets,typename ... GenericDrawLogics>
	class CombinedMeshDrawSet<GenericDrawSet<GenericDrawSets,GenericDrawLogics>...>:public BaseDrawSet
	{
	public:
		using storage_t = std::tuple<GenericDrawSet<GenericDrawSets, GenericDrawLogics>...>;
		CombinedMeshDrawSet(storage_t&& sets = storage_t{});
		void Render(RenderInterface&  the_interface)override
		{
			using index_seq_t = std::make_index_sequence<sizeof...(GenericDrawSets)>;
			Render(the_interface, index_seq_t{});
		}
	private:
		template<size_t ...Indices>
		void Render(RenderInterface& the_interface,std::index_sequence<Indices...>)
		{
			(std::get<Indices>(_draw_sets).Render(the_interface), ...);
		}
		storage_t _draw_sets;
	};

	template<typename Binding, typename DrawLogic>
	GenericDrawSet<Binding, DrawLogic>::GenericDrawSet(Binding binding, DrawLogic logic) : Base{ std::move(binding) }, _logic{ std::move(logic) }
	{
	}


	template<typename Binding, typename DrawLogic>
	GenericDrawSet<Binding, DrawLogic>::GenericDrawSet(DrawLogic logic) : GenericDrawSet{ Binding{},std::move(logic)}
	{
	}


	template<typename ...GenericDrawSets, typename ...GenericDrawLogics>
	CombinedMeshDrawSet<GenericDrawSet<GenericDrawSets, GenericDrawLogics>...>::CombinedMeshDrawSet(storage_t&& sets) : _draw_sets{ std::move(sets)}
	{
	}
	template<typename Binding,typename V>
	DrawSet<Binding,V>::DrawSet(Binding&& binding): _binding{std::move(binding)}
	{
	}


	class InstMeshDrawSet : public BaseDrawLogic
	{
	public:
		InstMeshDrawSet(span<const InstRenderObjects> inst_draw_range, vk::Buffer inst_mesh_render_buffer);
		void Render(RenderInterface& the_interface, bindings::RenderBindings& bindings)override;
	private:
		vk::Buffer _inst_mesh_render_buffer;
		span<const InstRenderObjects> _inst_draw_range;
	};

	class SkinnedMeshDrawSet : public BaseDrawLogic
	{
	public:
		SkinnedMeshDrawSet(span<const AnimatedRenderObject* const> inst_draw_range);
		void Render(RenderInterface& the_interface, bindings::RenderBindings& bindings)override;
	private:
		span<const AnimatedRenderObject* const>  _draw_calls;
	};

	class FsqDrawSet : public BaseDrawLogic
	{
	public:
		FsqDrawSet(MeshType mesh_type = MeshType::FSQ);
		void Render(RenderInterface& the_interface, bindings::RenderBindings& bindings)override;
	protected:
		bool BindRo(RenderInterface& the_interface, bindings::RenderBindings& bindings);
		RenderObject _fsq_ro;
		renderer_attributes _req = { {
			std::make_pair(vtx::Attrib::Position, 0),
			std::make_pair(vtx::Attrib::Normal, 1),
			std::make_pair(vtx::Attrib::UV, 2) }
		};
		MeshType _mesh_type;
	};

	class PerLightDrawSet : public FsqDrawSet
	{
	public:
		PerLightDrawSet();
		void Render(RenderInterface& the_interface, bindings::RenderBindings& bindings)override;
	private:
	};
}