#include "pch.h"

#include <vkn/GraphicsState.h>
#include <vkn/BufferHelpers.inl>
#include "ShadowBindings.h"
namespace idk::vkn::bindings
{

	bool ShadowFilter::Skip(RenderInterface&, const RenderObject& dc)
	{
		return !(dc.layer_mask & filter);
	}

	void ShadowFilter::SetState(const CameraData& cam)
	{
		filter = cam.culling_flags;
	}
	void GeometryShaderBinding::Bind(RenderInterface& the_interface)
	{
		the_interface.BindShader(ShaderStage::Geometry, geometry_shader);
	}
	bool PointShadowFilter::Skip(RenderInterface&, const RenderObject& dc)
	{
		return !(dc.layer_mask & filter);
	}
	void PointShadowFilter::SetState(const PointCameraData& cam)
	{
		filter = cam.culling_flags;
	}
	void FragmentShaderBinding::Bind(RenderInterface& the_interface)
	{
		the_interface.BindShader(ShaderStage::Fragment, fragment_shader);
	}
	void PointCameraViewportBindings::Bind(RenderInterface& the_interface)
	{
		the_interface.SetViewport(viewport);
		the_interface.SetScissors(viewport);
	}
	void PointCameraViewportBindings::SetState(const PointCameraData& cam) noexcept
	{
		viewport = cam.viewport;
	}
	void PointStandardVertexBindings::StateInfo::SetState(const PointCameraData& cam, const vector<SkeletonTransforms>& skel)
	{
		view_trf = cam.view_matrix;
		proj_trf = cam.projection_matrix;
		far_plane = cam.far_plane;
		light_pos = cam.pos;
		skeletons = &skel;
	}
	void PointStandardVertexBindings::SetState(const PointCameraData& camera, const vector<SkeletonTransforms>& skel)
	{
		state.SetState(camera, skel);
	}
	
	struct PointShadowData {
		float far_plane{};
		alignas(16) vec3 light_pos{0.f};
		FakeMat4<float> p[6];
	};
//#pragma optimize("", off)
	void PointStandardVertexBindings::Bind(RenderInterface& the_interface)
	{
		mat4 projection_trf = mat4{ 1,0,0,0,
							0,1,0,0,
							0,0,0.5f,0.5f,
							0,0,0,1
		};//map back into z: (0,1)
		PointShadowData pdata;
		pdata.far_plane = state.far_plane;
		pdata.light_pos = state.light_pos;

		auto i = 0;
		for (const auto& elem : state.proj_trf)
		{
			pdata.p[i] = projection_trf * elem;
			++i;
		}
		the_interface.BindUniform("CameraBlock", 0, hlp::to_data(pdata));
		the_interface.BindUniform("CameraBlock1", 0, hlp::to_data(pdata));
	}
	void PointStandardVertexBindings::Bind(RenderInterface& the_interface, const RenderObject& dc)
	{
		mat4 obj_trf = state.view_trf * dc.transform;
		mat4 obj_ivt = obj_trf.inverse().transpose();
		vector<mat4> mat4_block{ obj_trf,obj_ivt };
		the_interface.BindUniform("ObjectMat4Block", 0, hlp::to_data(mat4_block));
	}
	void PointStandardVertexBindings::Bind(RenderInterface& the_interface, const AnimatedRenderObject& dc)
	{
		Bind(the_interface, s_cast<const RenderObject&>(dc));
		BindAni(the_interface, dc);
	}
	void PointStandardVertexBindings::BindAni(RenderInterface& the_interface, const AnimatedRenderObject& dc)
	{
		the_interface.BindUniform("BoneMat4Block", 0, hlp::to_data((*state.skeletons)[dc.skeleton_index].bones_transforms));
	}
}