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
		RscHandle<ShaderProgram> brdf;
		RscHandle<ShaderProgram> convoluter;
		
		void Init() override = 0;
		void LateInit()override;
		void Shutdown() override = 0;
		
		//struct RenderBuffer;

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
		void PrepareLights(span<Light> lights);
		void BufferedLightData(vector<LightData>& out);
		void RenderObjData(vector<RenderObject>& out);
		void AnimatedRenderObjData(vector<AnimatedRenderObject>& out);

		virtual GraphicsAPI GetAPI() = 0;
	protected:
		struct RenderBuffer
		{
			vector<CameraData>   camera;
			vector<LightData>    lights;
			vector<CameraData>   light_camera_data;
			vector<RenderObject> mesh_render;
			vector<AnimatedRenderObject> skinned_mesh_render;
			vector<SkeletonTransforms> skeleton_transforms;

			CameraData  curr_scene_camera;

			RscHandle<ShaderProgram> mesh_vtx;
			RscHandle<ShaderProgram> skinned_mesh_vtx;
		};
		struct RenderPass
		{
			CameraData camera;
			std::optional<RscHandle<ShaderProgram>> overriding_vtx;
			std::optional<RscHandle<ShaderProgram>> overriding_geom;
			std::optional<RscHandle<ShaderProgram>> overriding_frag;
			//std::optional<RscHandle<FrameBuffer>>  output_buffer;
		};
		// triple buffered render state
		array<RenderBuffer, 3> object_buffer;
		unsigned               curr_write_buffer = 0;
		unsigned               curr_draw_buffer  = 1;
		bool                   write_buffer_dirty = false;

		RscHandle<ShaderProgram> mesh_vtx         = {};
		RscHandle<ShaderProgram> skinned_mesh_vtx = {};

		shared_ptr<pipeline_config> mesh_render_config{nullptr};

		void LoadShaders();
		virtual void LoadShaderImpl() {}
	private:
		void SwapWritingBuffer();
		void SubmitBuffers(RenderBuffer&& buffer);

	};
}