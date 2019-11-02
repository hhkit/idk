#pragma once
#include <idk.h>
#include <gfx/RenderObject.h>
#include <opengl/program/Program.h>
#include <opengl/program/PipelineProgram.h>
#include <opengl/system/FrameBufferManager.h>

#include <opengl/PixelData.h>

namespace idk::ogl
{
	class Win32GraphicsSystem;
	class OpenGLState
	{
	public:
		void Setup();
		~OpenGLState();
		void GenResources();

		void RenderDrawBuffer();

		void ConvoluteCubeMap(const RscHandle<ogl::OpenGLCubemap>& handle);
		void ComputeBRDF(const RscHandle<ogl::Program>&);

		PixelData PickData(const vec2& pos);
		void IsPicking();

	private:
		struct GPUState
		{
			CameraData                  camera_data;
			frustum                     camera_frustum;
			vector<LightData>           lights;
			
			RscHandle<ShaderProgram>    vtx_shader;
			renderer_attributes         renderer_attribs;

			RscHandle<Material>         material;
			RscHandle<MaterialInstance> instance;
			RscHandle<Mesh>             mesh;
		} curr_state;

		struct RendererBuffer
		{
			vector<mat4> model_view_transforms;
			vector<mat4> normal_transforms;
		} renderer_buffer;

		void SetCamera(const CameraData&);
		void SetLights(const vector<LightData>& original_data);
		bool PushVertexShader(RscHandle<ShaderProgram> vtx_shader, const renderer_attributes& attribs);
		bool PushMaterial(RscHandle<Material> vertex_shader);
		bool Push(RscHandle<MaterialInstance> material_instance);
		bool PushMesh(RscHandle<Mesh> mesh);
		void PushObjectTransform(const mat4& mat);
		void FlushBuffers();


		Win32GraphicsSystem* sys{};
		// variables
		PipelineProgram          pipeline;
		FrameBufferManager       fb_man;


		// vaos
		GLuint                   vao_id = 0;
		GLuint                   particle_vao_id = 0;
		GLuint                   font_vao_id = 0;
		GLuint					 vbo_font_id = 0;

		GLuint mesh_batch_object_transform_fbo = 0;
		GLuint mesh_batch_normal_transform_fbo = 0;

		RscHandle<OpenGLTexture> brdf_texture;
		bool is_picking = false;
	};
}