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
		struct MeshEntry
		{
			MeshEntry() = default;

			MeshEntry(unsigned base_v, unsigned base_i, unsigned num_i, unsigned text_index);

			unsigned _base_vertex = 0;
			unsigned _base_index = 0;
			unsigned _num_index = 0;
			unsigned _texture_index = 0;
		};

		OpenGLMesh() = default;
		OpenGLMesh(OpenGLMesh&&);
		OpenGLMesh& operator=(OpenGLMesh&&);

		OpenGLMesh(const vector<MeshEntry>& entries);

		OpenGLMesh(const OpenGLMesh&) = delete;
		OpenGLMesh& operator=(const OpenGLMesh&) = delete;

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
		void   AddMeshEntry(unsigned base_v, unsigned base_i, unsigned num_i, unsigned text_index);
		span<MeshEntry> GetMeshEntries();
	private:
		vector<MeshEntry>	 _mesh_entries;
		vector<OpenGLBuffer> _buffers;
		OpenGLBuffer         _element_array_object;
		GLenum               _draw_mode = GL_TRIANGLES;
	};
}