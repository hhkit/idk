#pragma once
#include <idk.h>
#include <core/ISystem.h>
#include <ds/span.h>
#include <gfx/RenderObject.h>
#include <gfx/pipeline_config.h>
#include <gfx/Camera.h>
#include <gfx/Light.h>

namespace idk
{
	enum class GraphicsAPI
	{
		OpenGL,
		//	DirectX,
		Vulkan,
		Default = OpenGL
	};

	class GraphicsSystem
		: public ISystem
	{
	public:
		RscHandle<ShaderProgram> convoluter;
		
		void Init() override = 0;
		void Shutdown() override = 0;
		

		//Converts the Renderers and transforms stores the render data into a buffer
		virtual void BufferGraphicsState(
			span<class MeshRenderer>,
			span<Animator> animators,
			span<SkinnedMeshRenderer> skinned_mesh_renderers,
			span<const class Transform>, 
			span<const Camera> camera, 
			span<const Light> lights);
		virtual void Prerender() {};
		virtual void RenderRenderBuffer() = 0;
		virtual void SwapBuffer() = 0;
		virtual GraphicsAPI GetAPI() = 0;

		bool editorExist{false};
	protected:
		struct RenderBuffer
		{
			vector<CameraData>   camera;
			vector<LightData>    lights;
			vector<RenderObject> mesh_render;
			vector<AnimatedRenderObject> skinned_mesh_render;
			vector<SkeletonTransforms> skeleton_transforms;
		};
		// triple buffered render state
		array<RenderBuffer, 3> object_buffer;
		unsigned               curr_write_buffer = 0;
		unsigned               curr_draw_buffer  = 1;
		bool                   write_buffer_dirty = false;

		shared_ptr<pipeline_config> mesh_render_config{nullptr};
	private:
		void SwapWritingBuffer();
		void SubmitBuffers(RenderBuffer&& buffer);

	};
}