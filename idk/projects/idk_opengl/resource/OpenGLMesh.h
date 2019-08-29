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

		GLenum GetDrawMode() const;
		void   SetDrawMode(GLenum);
		int    GetAttribs() const override;

		void   Bind(const renderer_reqs& locations);
		void   Draw();
		void   Reset();
		void   AddBuffer(OpenGLBuffer&);
		void   AddMeshEntry(unsigned base_v, unsigned base_i, unsigned num_i, unsigned text_index);
	private:
		struct MeshEntry
		{
			unsigned base_vertex;
			unsigned base_index;
			unsigned num_index;
			unsigned texture_index;
		};

		vector<MeshEntry>	 _mesh_entries;
		vector<OpenGLBuffer> _buffers;
		OpenGLBuffer         _element_array_object;
		GLenum               _draw_mode = GL_TRIANGLES;
	};
}