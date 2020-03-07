#pragma once
#include <vkn/RenderBindings.h>
#include <vkn/CameraViewportBindings.h>
#include <vkn/StandardVertexBindings.h>
namespace idk::vkn::bindings
{
	struct GeometryShaderBinding : RenderBindings
	{
		RscHandle<ShaderProgram> geometry_shader;
		void Bind(RenderInterface& the_interface)override;
	};
	struct FragmentShaderBinding : RenderBindings
	{
		RscHandle<ShaderProgram> fragment_shader;
		void Bind(RenderInterface& the_interface)override;
	};
	struct ShadowFilter :RenderBindings
	{
		LayerMask filter;
		bool Skip(RenderInterface& the_interface, const  RenderObject& dc) override;
		void SetState(const CameraData& cam);
	};
	struct PointShadowFilter :RenderBindings
	{
		LayerMask filter;
		bool Skip(RenderInterface& the_interface, const  RenderObject& dc) override;
		void SetState(const PointCameraData& cam);
	};

	struct PointStandardVertexBindings : RenderBindings
	{
		//const GraphicsState* _state;
		//const GraphicsState& State();
		struct StateInfo
		{
			float far_plane;
			vec3 light_pos{ 0.f };
			const vector<SkeletonTransforms>* skeletons;
			mat4 view_trf;
			vector<mat4> proj_trf;
			void SetState(const PointCameraData& camera, const vector<SkeletonTransforms>& skel);
		};
		StateInfo state;
		void SetState(const PointCameraData& camera, const vector<SkeletonTransforms>& skel);

		void Bind(RenderInterface& the_interface)override;
		void Bind(RenderInterface& the_interface, const RenderObject& dc)override;
		void Bind(RenderInterface& the_interface, const  AnimatedRenderObject& dc);
		void BindAni(RenderInterface& the_interface, const AnimatedRenderObject& dc)override;

	};
	class PointCameraViewportBindings : public RenderBindings
	{
	public:
		rect viewport;
		void Bind(RenderInterface& the_interface) override;
		void SetState(const PointCameraData& cam) noexcept;
	};
	namespace defaults
	{
	}
	using ShadowBinding = CombinedBindings<ShadowFilter, CameraViewportBindings,VertexShaderBinding,StandardVertexBindings>;
	using PointShadowBinding = CombinedBindings<PointShadowFilter, PointCameraViewportBindings, VertexShaderBinding, GeometryShaderBinding, FragmentShaderBinding, PointStandardVertexBindings>;
}