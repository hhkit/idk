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
		glDrawElements(_draw_mode, _element_array_object.count(), GL_UNSIGNED_INT, 0);
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
}
