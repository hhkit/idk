#pragma once
#include <vkn/RenderBindings.h>
#include <util/PoolContainer.h>
namespace idk::vkn::bindings
{
	struct VertexShaderBinding : RenderBindings
	{
		RscHandle<ShaderProgram> vertex_shader;
		void Bind(RenderInterface& the_interface)override;
	};

	//Standard binding for vertex stuff
	struct StandardVertexBindings : RenderBindings
	{
		//const GraphicsState* _state;
		//const GraphicsState& State();
		struct StateInfo
		{
			const vector<SkeletonTransforms>* skeletons=nullptr;
			mat4 view_trf, proj_trf;
			void SetState(const GraphicsState& vstate);
			void SetState(const CameraData& camera, const vector<SkeletonTransforms>& skel);
		};
		StateInfo state;
		void SetState(const GraphicsState& vstate);
		void SetState(const CameraData& camera, const vector<SkeletonTransforms>& skel);

		struct UboCache
		{
			vk::Buffer buffer;
			uint32_t offset;
		};

		PooledContainer<std::vector<UboCache>> cache_;

		void Bind(RenderInterface& the_interface)override;
		void Bind(RenderInterface& the_interface, const RenderObject& dc)override;
		void PrepareBindRange(RenderInterface& the_interface, strided_span<const RenderObject> dc,span<const size_t> indices)override;
		void Bind(RenderInterface& the_interface, const  AnimatedRenderObject& dc);
		void BindAni(RenderInterface& the_interface, const AnimatedRenderObject& dc)override;

	};

}