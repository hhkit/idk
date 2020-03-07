#include "pch.h"
#include "StandardVertexBindings.h"

#include <vkn/GraphicsState.h>
#include <vkn/BufferHelpers.inl>
#include "ShadowBindings.h"

namespace idk::vkn::bindings
{

	void StandardVertexBindings::SetState(const GraphicsState& vstate) {
		auto& cam = vstate.camera;
		SetState(cam, vstate.GetSkeletonTransforms());
	}

	void StandardVertexBindings::SetState(const CameraData& cam, const vector<SkeletonTransforms>& skel)
	{
		state.SetState(cam, skel);
	}

	void StandardVertexBindings::Bind(RenderInterface& the_interface)
	{
		//map back into z: (0,1)
		mat4 projection_trf = mat4{ 1,0,0,0,
							0,1,0,0,
							0,0,0.5f,0.5f,
							0,0,0,1
		}*state.proj_trf;//map back into z: (0,1)
		the_interface.BindUniform("CameraBlock", 0, hlp::to_data(projection_trf));
	}

	void StandardVertexBindings::Bind(RenderInterface& the_interface, const RenderObject& dc)
	{
		mat4 obj_trf = state.view_trf * dc.transform;
		mat4 obj_ivt = obj_trf.inverse().transpose();
		vector<mat4> mat4_block{ obj_trf,obj_ivt };
		the_interface.BindUniform("ObjectMat4Block", 0, hlp::to_data(mat4_block));
	}

	void StandardVertexBindings::Bind(RenderInterface& the_interface, const AnimatedRenderObject& dc)
	{
		Bind(the_interface, s_cast<const RenderObject&>(dc));
		BindAni(the_interface, dc);
	}

	void StandardVertexBindings::BindAni(RenderInterface& the_interface, const AnimatedRenderObject& dc)
	{
		//auto& state = State();
		the_interface.BindUniform("BoneMat4Block", 0, hlp::to_data((*state.skeletons)[dc.skeleton_index].bones_transforms));
	}
	void VertexShaderBinding::Bind(RenderInterface& the_interface)
	{
		the_interface.BindShader(ShaderStage::Vertex, vertex_shader);
	}
	void StandardVertexBindings::StateInfo::SetState(const GraphicsState& vstate)
	{
		SetState(vstate.camera, *vstate.skeleton_transforms);
	}
	void StandardVertexBindings::StateInfo::SetState(const CameraData& cam, const vector<SkeletonTransforms>& skel)
	{
		view_trf = cam.view_matrix;
		proj_trf = cam.projection_matrix;
		skeletons = &skel;
	}
}