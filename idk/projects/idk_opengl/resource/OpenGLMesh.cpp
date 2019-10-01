#include "pch.h"
#include "OpenGLMesh.h"

namespace idk::ogl
{
	OpenGLMesh::OpenGLMesh(OpenGLMesh&& rhs)
		:	_mesh_entries			{ std::move(rhs._mesh_entries) }, 
			_buffers				{ std::move(rhs._buffers) }, 
			_element_array_object	{ std::move(rhs._element_array_object) }, 
			_draw_mode				{ rhs._draw_mode }
	{
	}

	OpenGLMesh& OpenGLMesh::operator=(OpenGLMesh&& rhs)
	{
		std::swap(_mesh_entries,		 rhs._mesh_entries);
		std::swap(_buffers,				 rhs._buffers);
		std::swap(_element_array_object, rhs._element_array_object);
		std::swap(_draw_mode,			 rhs._draw_mode);
		
		return *this;
	}

	OpenGLMesh::OpenGLMesh(const vector<OpenGLMesh::MeshEntry>& entries)
		:_mesh_entries{ entries }
	{
	}

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
		// for (auto& entry : _mesh_entries)
		// {
		// 	glDrawElements( _draw_mode, static_cast<GLsizei>(entry._num_index), GL_UNSIGNED_INT, (void*)(sizeof(unsigned) * entry._base_index));
		// }
		glDrawElements(_draw_mode, _element_array_object.count(), GL_UNSIGNED_INT, 0);
	}

	void OpenGLMesh::BindAndDraw(const renderer_reqs& locations)
	{
		Bind(locations);
		Draw();
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
			break;
		case GL_ARRAY_BUFFER:
		default:
			_buffers.emplace_back(std::move(buf));
		}
	}

	void OpenGLMesh::AddMeshEntry(unsigned base_v, unsigned base_i, unsigned num_i, unsigned text_index)
	{
		_mesh_entries.emplace_back(MeshEntry{ base_v, base_i, num_i, text_index });
	}

	span<OpenGLMesh::MeshEntry> OpenGLMesh::GetMeshEntries()
	{
		return span<MeshEntry>(_mesh_entries);
	}

	OpenGLMesh::MeshEntry::MeshEntry(unsigned base_v, unsigned base_i, unsigned num_i, unsigned text_index)
		:_base_vertex{ base_v }, _base_index{ base_i }, _num_index{ num_i }, _texture_index{ text_index }
	{
	}
}
