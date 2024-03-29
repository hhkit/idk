#pragma once
#include <idk.h>
#include <gfx/RenderObject.h>
#include <opengl/program/Program.h>
#include <opengl/program/PipelineProgram.h>
#include <opengl/system/FrameBufferManager.h>
#include <opengl/resource/OpenGLBuffer.h>

#include <opengl/PixelData.h>

namespace idk
{
	struct LightData;
}
namespace idk::ogl
{
	class OpenGLMesh;
	class Win32GraphicsSystem;
	class OpenGLState
	{
	public:
		OpenGLState() = default;
		~OpenGLState();
		void Setup();
		void GenResources();

		void RenderDrawBuffer();

		void ConvoluteCubeMap(const RscHandle<ogl::OpenGLCubemap>& handle);
		void ComputeBRDF(const RscHandle<ogl::Program>&);

		PixelData PickData(const vec2& pos);
		void IsPicking();

		void LoadShaderImpl();
	private:
		Win32GraphicsSystem* sys{};
		// variables
		PipelineProgram          pipeline;
		FrameBufferManager       fb_man;
		GLuint                   vao_id = 0;
		GLuint                   particle_vao_id = 0;
		GLuint                   font_vao_id = 0;
		GLuint					 vbo_font_id = 0;

		GLuint object_vbo_id = 0;
		GLuint normal_vbo_id = 0;
		size_t num_inst_bytes;
		vector<mat4> object_transforms;
		vector<mat4> normal_transforms;

		RscHandle<OpenGLTexture> brdf_texture;
		//RscHandle<ShaderProgram> debug_fragment;
		bool is_picking = false;

		RscHandle<Material> curr_mat;
		RscHandle<MaterialInstance> curr_mat_inst;
		RscHandle<OpenGLMesh> curr_mesh;
		GLuint material_texture_uniforms = 0;
		CameraData curr_cam;
		span<LightData> curr_lights;
        OpenGLBuffer particle_buf{ GL_ARRAY_BUFFER, {
            { vtx::Attrib::ParticlePosition, sizeof(ParticleObj), offsetof(ParticleObj, position) },
            { vtx::Attrib::ParticleRotation, sizeof(ParticleObj), offsetof(ParticleObj, rotation) },
            { vtx::Attrib::ParticleSize, sizeof(ParticleObj), offsetof(ParticleObj, size) },
            { vtx::Attrib::Color, sizeof(ParticleObj), offsetof(ParticleObj, color) }
        } };

		void BindMaterial(const RscHandle<Material>& mat);
		void PushMaterial(const RscHandle<Material>& mat);
		void BindMaterialInstance(const RscHandle<MaterialInstance>& mat_inst);
		void PushMaterialInstance(const RscHandle<MaterialInstance>& mat_inst);
		void PushMesh(const RscHandle<OpenGLMesh>& mesh);
		void PushObjectTransform(const mat4& tfm);
		void FlushObjectTransforms();
		void UpdateInstancedBuffers(const void* data, size_t num_bytes);
		void UseInstancedBuffers(size_t starting_instance);
		
	};
}