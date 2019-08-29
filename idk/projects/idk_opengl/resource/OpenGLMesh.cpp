#include "pch.h"
#include "OpenGLMesh.h"

namespace idk::ogl
{
	GLenum OpenGLMesh::GetDrawMode() const
	{
		return _draw_mode;
	}

	void OpenGLMesh::SetDrawMode(GLenum draw_mode)
	{
		_draw_mode = draw_mode;
	}

	int OpenGLMesh::GetAttribs() const
	{
		int retval{};
		for (auto& elem : _buffers)
		{
			for (auto& attrib : elem.descriptor)
				retval |= 1 << attrib.attrib;
		}
		return retval;
	}

	void OpenGLMesh::Bind(const renderer_reqs& locations)
	{
		for (auto& elem : _buffers)
			elem.BindForDraw(locations);
	}

#pragma warning(disable:4312)

	void OpenGLMesh::Draw()
	{
		_element_array_object.Bind();
		for (auto& entry : _mesh_entries)
		{
			glDrawElementsBaseVertex( _draw_mode, static_cast<GLsizei>(entry.num_index), GL_UNSIGNED_INT, (void*)(sizeof(unsigned) * entry.base_index), entry.base_vertex);
		}
		// glDrawElements(_draw_mode, _element_array_object.count(), GL_UNSIGNED_INT, 0);
	}

	void OpenGLMesh::Reset()
	{
		_buffers.clear();
		_mesh_entries.clear();
		_element_array_object = OpenGLBuffer{};
	}

	void OpenGLMesh::AddBuffer(OpenGLBuffer& buf)
	{
		switch(buf.type())
		{
		case GL_ELEMENT_ARRAY_BUFFER:
			_element_array_object = std::move(buf);
		case GL_ARRAY_BUFFER:
		default:
			_buffers.emplace_back(std::move(buf));
		}
	}

	void OpenGLMesh::AddMeshEntry(unsigned base_v, unsigned base_i, unsigned num_i, unsigned text_index)
	{
		_mesh_entries.emplace_back(MeshEntry{ base_v, base_i, num_i, text_index });
	}
}
