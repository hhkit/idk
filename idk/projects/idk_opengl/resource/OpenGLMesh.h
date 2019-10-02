#pragma once
#include <idk.h>

#include <glad/glad.h>

#include <gfx/Mesh.h>
#include <gfx/vertex_descriptor.h>
#include <idk_opengl/resource/OpenGLTypeDescriptors.h>
#include <idk_opengl/resource/OpenGLBuffer.h>

namespace idk::ogl
{
	class OpenGLMesh
		: public Mesh
	{
	public:

		OpenGLMesh() = default;
		OpenGLMesh(OpenGLMesh&&);
		OpenGLMesh& operator=(OpenGLMesh&&);
		
		OpenGLMesh(const OpenGLMesh&)				= delete;
		OpenGLMesh& operator=(const OpenGLMesh&)	= delete;

		GLenum GetDrawMode() const;
		void   SetDrawMode(GLenum);
		int    GetAttribs() const override;

		void   Bind(const renderer_reqs& locations);
		void   Draw();
		void   BindAndDraw(const renderer_reqs& locations);
		template<typename T>
		void   BindAndDraw() { BindAndDraw(T::GetRequiredAttributes()); }
		void   Reset();

		void   AddBuffer(OpenGLBuffer&);
		
	private:
		vector<OpenGLBuffer> _buffers;
		OpenGLBuffer         _element_array_object;
		GLenum               _draw_mode = GL_TRIANGLES;
	};
}