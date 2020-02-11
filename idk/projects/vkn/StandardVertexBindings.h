#pragma once
#include <vkn/RenderBindings.h>
namespace idk::vkn::bindings
{

	//Standard binding for vertex stuff
	struct StandardVertexBindings : RenderBindings
	{
		//const GraphicsState* _state;
		//const GraphicsState& State();
		const vector<SkeletonTransforms>* skeletons;
		mat4 view_trf, proj_trf;
		void SetState(const GraphicsState& vstate);
		void SetState(const CameraData& camera, const vector<SkeletonTransforms>& skel);

		void Bind(RenderInterface& the_interface)override;
		void Bind(RenderInterface& the_interface, const RenderObject& dc)override;
		void Bind(RenderInterface& the_interface, const  AnimatedRenderObject& dc);
		void BindAni(RenderInterface& the_interface, const AnimatedRenderObject& dc)override;

	};

}