#pragma once
#include <idk.h>
#include <gfx/Camera.h>
#include <vkn/utils/utils.inl>
#include <vkn/RenderInterface.h>
namespace idk
{
	struct RenderObject;
	struct AnimatedRenderObject;
}
namespace idk::vkn
{
	struct GraphicsState;
	namespace bindings
	{

	enum class PbrCubeMapVars
	{
		eIrradiance,
		eEnvironmentProbe,
	};
	using PbrCubeMapVarsPack = meta::enum_pack< PbrCubeMapVars,
		PbrCubeMapVars::eIrradiance,
		PbrCubeMapVars::eEnvironmentProbe
	>;
	using PbrCubeMapVarsInfo = meta::enum_info < PbrCubeMapVars, PbrCubeMapVarsPack>;
	enum class PbrTexVars
	{
		eBrdfLut, //Brdf LookUp Table
	};
	using PbrTexVarsPack = meta::enum_pack< PbrTexVars,
		PbrTexVars::eBrdfLut //Brdf LookUp Table
	>;
	using PbrTexVarsInfo = meta::enum_info < PbrTexVars, PbrTexVarsPack>;


	struct RenderBindings
	{
		virtual bool Skip(RenderInterface&, const  RenderObject&) { return false; }
		//Stuff that should be bound at the start, before the renderobject/animated renderobject loop.
		virtual void Bind(RenderInterface& the_interface);
		//Stuff that needs to be bound with every renderobject/animated renderobject
		virtual void Bind(RenderInterface& the_interface, const  RenderObject& dc);
		//Stuff that needs to be bound with every animated renderobject (default behaviour invokes 
		// void Bind(RenderInterface& the_interface, const  RenderObject& dc)
		//before invoking BindAni 
		//(this distinction is really just for overridable convenience. We could force everyone
		// who needs to write a new binder to manually invoke the RenderObject bind on their own.)
		virtual void Bind(RenderInterface& the_interface, const AnimatedRenderObject& dc);
		//Stuff that needs to be bound only for animated renderobject.
		virtual void BindAni(RenderInterface& the_interface, const AnimatedRenderObject& dc);

		//Stuff that needs to be bound only for font renderobject
		virtual void BindFont(RenderInterface& the_interface, const FontRenderData& dc);

		//Stuff that needs to be bound only for canvas renderobject
		virtual void BindCanvas(RenderInterface& the_interface, const TextData& dc, const UIRenderObject& dc_one);
		virtual void BindCanvas(RenderInterface& the_interface, const ImageData& dc, const UIRenderObject& dc_one);


	};

	//A combined set of binders that will bind in the order that is specified in the template arguments.
	template<typename ...Args>
	struct CombinedBindings :RenderBindings
	{
		std::tuple<Args...> binders;
		bool Skip(RenderInterface& the_interface, const  RenderObject& dc) override;
		//for each binder, execute f(binder,FArgs...) if cond<decltype(binder)> is true.
		template<template<typename...>typename cond = meta::always_true_va, typename Fn, typename ...FArgs >
		void for_each_binder(Fn&& f, FArgs& ...args);
		void Bind(RenderInterface& the_interface) override;
		void Bind(RenderInterface& the_interface, const RenderObject& dc) override;
		void BindAni(RenderInterface& the_interface, const  AnimatedRenderObject& dc) override;
	};

	}
}