#pragma once
#include <idk.h>
#include <core/ISystem.h>
#include <ds/span.h>
#include <gfx/RenderObject.h>
#include <gfx/Camera.h>
namespace idk
{
	//Temp Usings, replace when implementations are in.
	using RenderTexHandle = uint32_t;
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
		void Init() override = 0;
		void Shutdown() override = 0;
		//Converts the Renderers and transforms stores the render data into a buffer
		void SetMainCamera(Handle<Camera> camera) { _main_camera = camera; }
		Handle<Camera> CurrentCamera() { return _main_camera; };
		virtual void BufferGraphicsState(span<class MeshRenderer>, span<const class Transform>, span<const Camera> camera);
		virtual void RenderRenderBuffer() = 0;
		virtual void SwapBuffer() = 0;
		virtual GraphicsAPI GetAPI() = 0;
	protected:
		struct RenderBuffer
		{
			vector<CameraData>   camera;
			vector<RenderObject> mesh_render;
			vector<RenderObject> skinned_mesh_render;
		};
		using GraphicsState = RenderBuffer;
		// triple buffered render state
		array<RenderBuffer, 3> object_buffer;
		unsigned               curr_write_buffer = 0;
		unsigned               curr_draw_buffer  = 1;
		bool                   write_buffer_dirty = false;

		// main camera
		Handle<Camera> _main_camera;
		//Handle<GameObject> _main_camera;
	private:
		void SwapWritingBuffer();
		void SubmitBuffers(RenderBuffer&& buffer);

	};
}