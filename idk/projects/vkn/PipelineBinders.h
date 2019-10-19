#pragma once
#include <idk.h>
#include <vkn/utils/utils.inl>
#include <gfx/Camera.h>
namespace idk
{
	struct RenderObject;
	struct AnimatedRenderObject;

	namespace vkn
	{
		class  PipelineThingy;
		struct GraphicsState;

		struct StandardBindings
		{
			//Stuff that should be bound at the start, before the renderobject/animated renderobject loop.
			virtual void Bind(PipelineThingy& the_interface);
			//Stuff that needs to be bound with every renderobject/animated renderobject
			virtual void Bind(PipelineThingy& the_interface, const  RenderObject& dc);
			//Stuff that needs to be bound with every animated renderobject (default behaviour invokes 
			// void Bind(PipelineThingy& the_interface, const  RenderObject& dc)
			//before invoking BindAni 
			//(this distinction is really just for overridable convenience. We could force everyone
			// who needs to write a new binder to manually invoke the RenderObject bind on their own.)
			virtual void Bind(PipelineThingy& the_interface, const AnimatedRenderObject& dc);
			//Stuff that needs to be bound only for animated renderobject.
			virtual void BindAni(PipelineThingy& the_interface, const AnimatedRenderObject& dc);
		};

		//Standard binding for vertex stuff
		struct StandardVertexBindings : StandardBindings
		{
			//const GraphicsState* _state;
			//const GraphicsState& State();
			const vector<SkeletonTransforms>* skeletons;
			mat4 view_trf, proj_trf;
			void SetState(const GraphicsState& vstate);
			void SetState(const CameraData& camera, const vector<SkeletonTransforms>& skel);

			void Bind(PipelineThingy& the_interface)override;
			void Bind(PipelineThingy& the_interface, const RenderObject& dc)override;
			void Bind(PipelineThingy& the_interface, const  AnimatedRenderObject& dc);
			void BindAni(PipelineThingy& the_interface, const AnimatedRenderObject& dc)override;

		};
		struct PbrFwdBindings : StandardBindings
		{
			const GraphicsState* _state;
			CameraData cam;
			const GraphicsState& State();
			string light_block;
			mat4 view_trf, pbr_trf, proj_trf;
			void SetState(const GraphicsState& vstate);

			void Bind(PipelineThingy& the_interface, const RenderObject& dc)override;
		};

		struct StandardMaterialFragBindings : StandardBindings
		{
			//Assumes that the material is valid.
			void Bind(PipelineThingy& the_interface, const  RenderObject& dc)override;

		};
		struct StandardMaterialBindings : StandardBindings
		{
			const GraphicsState* _state;
			const GraphicsState& State() { return *_state; }
			void SetState(const GraphicsState& vstate);

			//Assumes that the material is valid.
			void Bind(PipelineThingy& the_interface, const RenderObject& dc) override;

			void BindAni(PipelineThingy& the_interface, const AnimatedRenderObject& dc)override;

		};
		//A combined set of binders that will bind in the order that is specified in the template arguments.
		template<typename ...Args>
		struct CombinedBindings :StandardBindings
		{
			std::tuple<Args...> binders;
			//for each binder, execute f(binder,FArgs...) if cond<decltype(binder)> is true.
			template<template<typename...>typename cond = meta::always_true_va, typename Fn, typename ...FArgs >
			void for_each_binder(Fn&& f, FArgs& ...args);
			void Bind(PipelineThingy& the_interface) override;
			void Bind(PipelineThingy& the_interface, const RenderObject& dc) override;
			void BindAni(PipelineThingy& the_interface, const  AnimatedRenderObject& dc) override;
		};
		using PbrFwdMaterialBinding = CombinedBindings<StandardVertexBindings, StandardMaterialFragBindings, PbrFwdBindings, StandardMaterialBindings>;
		using ShadowBinding = CombinedBindings<StandardVertexBindings>;
	}
}