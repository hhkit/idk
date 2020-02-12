#pragma once
#include <vkn/RenderBindings.h>
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
		GenericDrawSet(Binding, DrawLogic);
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
		CombinedMeshDrawSet(GenericDrawSet<GenericDrawSets, GenericDrawLogics>&&... sets);
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
		std::tuple<GenericDrawSet<GenericDrawSets, GenericDrawLogics>...> _draw_sets;
	};

	template<typename Binding, typename DrawLogic>
	GenericDrawSet<Binding, DrawLogic>::GenericDrawSet(Binding binding, DrawLogic logic) : DrawSet{ std::move(binding) }, _logic{ std::move(logic) }
	{
	}

	template<typename ...GenericDrawSets, typename ...GenericDrawLogics>
	CombinedMeshDrawSet<GenericDrawSet<GenericDrawSets, GenericDrawLogics>...>::CombinedMeshDrawSet(GenericDrawSet<GenericDrawSets, GenericDrawLogics>&& ...sets) : _draw_sets{ std::move(sets)... }
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
		SkinnedMeshDrawSet(span<const AnimatedRenderObject*> inst_draw_range);
		void Render(RenderInterface& the_interface, bindings::RenderBindings& bindings)override;
	private:
		span<const AnimatedRenderObject*>  _draw_calls;
	};
}