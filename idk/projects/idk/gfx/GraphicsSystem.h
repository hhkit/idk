#pragma once
#include <idk.h>
#include <core/ISystem.h>
#include <ds/span.h>
#include <gfx/RenderObject.h>
#include <gfx/pipeline_config.h>
#include <gfx/Camera.h>
#include <gfx/Light.h>
#include <particle/ParticleData.h>
#include <gfx/FontData.h>
#include <gfx/RenderRequest.h>

namespace idk
{

	enum class GraphicsAPI
	{
		OpenGL,
		//	DirectX,
		Vulkan,
		Default = OpenGL
	};

	enum VertexShaders
	{
		VDebug,
		VNormalMesh,
		VSkinnedMesh,
        VParticle,
		VSkyBox,
		VPBRConvolute,
		VFsq,
		VFont,
        VUi,
		VMax
	};
	enum FragmentShaders
	{
		FDebug,
		FSkyBox,
		FShadow,
		FPicking,
		FPBRConvolute,
		FBrdf,
		FFont,
		FDeferredPost,
		FDeferredPostSpecular,
		FDeferredPostAmbient,
		FMax
	};
	enum GeometryShaders
	{
		GSinglePassCube,
		GMax
	};
	struct RenderRequest;

	class GraphicsSystem
		: public ISystem
	{
	public:
		struct RenderRange
		{
			CameraData camera;
			size_t inst_mesh_render_begin{}, inst_mesh_render_end{};
			size_t inst_particle_begin{}, inst_particle_end{};
			size_t inst_font_begin{}, inst_font_end{};
		};
		struct LightRenderRange
		{
			size_t light_index;
			size_t inst_mesh_render_begin{}, inst_mesh_render_end{};
			size_t instanced_skinned_mesh_render_begin{}, instanced_skinned_mesh_render_end{};
		};
		struct CanvasRenderRange
		{
			size_t inst_font_begin{}, inst_font_end{};
		};

		//RscHandle<ShaderProgram> brdf;
		//RscHandle<ShaderProgram> convoluter;
		bool isolate = false;
		array<RscHandle<ShaderProgram>, VertexShaders::VMax>   renderer_vertex_shaders;
		array<RscHandle<ShaderProgram>, FragmentShaders::FMax>   renderer_fragment_shaders;
		array<RscHandle<ShaderProgram>, GeometryShaders::GMax>   renderer_geometry_shaders;
		
		void Init() override = 0;
		void LateInit()override;
		void Shutdown() override = 0;
		
		//struct RenderBuffer;

		//Converts the Renderers and transforms stores the render data into a buffer
		void SortCameras();
		virtual void BufferGraphicsState(
			span<class MeshRenderer>,
			span<Animator> animators,
			span<SkinnedMeshRenderer> skinned_mesh_renderers,
            span<class ParticleSystem>,
			span<class TextMesh>,
			span<class Text>,
            span<class Image>,
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
		void PrepareSkeletonTransforms(const vector<Handle<Animator>>& animators, vector<SkeletonTransforms>& buffer_out, hash_table<Handle<Animator>, size_t>& index_table_out);
		void PrepareSkeletonTransform(const Animator& animator, vector<SkeletonTransforms>& buffer_out, hash_table<Handle<Animator>,size_t>& index_table_out);
		std::optional<AnimatedRenderObject> GetAnimatedRenderObj(const SkinnedMeshRenderer& skinned_mesh_renderer, const hash_table<Handle<Animator>, size_t>& index_table);
		std::optional<RenderObject> GetRenderObj(const MeshRenderer& skinned_mesh_renderer);

		bool is_deferred()const;
		bool is_deferred(bool enable);

		size_t AddRenderRequest(RenderRequest&& request);

		bool RenderRequestStatus(size_t index);

		shared_ptr<pipeline_config> MeshRenderConfig()const { return mesh_render_config; }

		virtual GraphicsAPI GetAPI() = 0;
		void LoadShaders();
	protected:
		struct SpecialRenderBuffer
		{
			CameraData camera;
			vector<RenderObject> mesh_render;
			vector<AnimatedRenderObject> skinned_mesh_render;
			vector<SkeletonTransforms> skeleton_transforms;
			bool done_flag=false;
			bool valid_flag = false;
			RscHandle<ShaderProgram> mesh_vtx;
			RscHandle<ShaderProgram> skinned_mesh_vtx;
		};

		//Todo: change to a thing with a free list thing.
		pool<SpecialRenderBuffer> render_requests;

		struct RenderBuffer
		{
			vector<CameraData>   camera;
			vector<LightData>    lights;
			vector<CameraData>   light_camera_data;
			vector<RenderObject> mesh_render;
			vector<AnimatedRenderObject> skinned_mesh_render;
			vector<SkeletonTransforms> skeleton_transforms;
            vector<ParticleRenderData> particle_render_data;
			vector<ParticleObj>        particle_buffer;
			vector<ParticleRange>      particle_range;
			vector<FontRenderData>	 font_render_data;
			//vector<FontArrayData>    font_array_data;
			vector<FontPoint>     font_buffer;
			vector<FontRange>      font_range;

			hash_table<Handle<Canvas>, vector<UIRenderObject>> ui_render_per_canvas;
			vector<UIRenderObjectWithCanvas> ui_canvas;
			vector<UITextRange>   ui_text_range;
			vector<UIAttriBlock>   ui_text_buffer;
			size_t  ui_total_num_of_text{ 0 };
			//vector<FontPoint>     ui_text_buffer;

			size_t curr_scene_camera_index;

			vector<InstRenderObjects> instanced_mesh_render;
			//vector<InstAnimatedRenderObjects> instanced_skinned_mesh_render;

			vector<InstancedData> inst_mesh_render_buffer;
			//vector<AnimatedInstancedData> inst_skinned_mesh_render_buffer;

			vector<RenderRange> culled_render_range;
			vector<LightRenderRange> culled_light_render_range;
			vector<CanvasRenderRange> canvas_render_range;

			//RscHandle<ShaderProgram> mesh_vtx;
			//RscHandle<ShaderProgram> skinned_mesh_vtx;
			array<RscHandle<ShaderProgram>, VertexShaders::VMax>   renderer_vertex_shaders;
			array<RscHandle<ShaderProgram>, FragmentShaders::FMax>   renderer_fragment_shaders;
		};
		// triple buffered render state
		array<RenderBuffer, 3> object_buffer;
		unsigned               curr_write_buffer = 0;
		unsigned               curr_draw_buffer  = 1;
		bool                   write_buffer_dirty = false;

		//RscHandle<ShaderProgram> mesh_vtx         = {};
		//RscHandle<ShaderProgram> skinned_mesh_vtx = {};

		shared_ptr<pipeline_config> mesh_render_config{nullptr};

		virtual void LoadShaderImpl() {}
	private:

		void SwapWritingBuffer();
		void SubmitBuffers(RenderBuffer&& buffer);
		RenderBuffer& GetWriteBuffer();
		bool _is_deferred = false;
	};
}