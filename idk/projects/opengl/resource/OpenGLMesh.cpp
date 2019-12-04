#include "pch.h"
#include "OpenGLMesh.h"

namespace idk::ogl
{
	OpenGLMesh::OpenGLMesh(const CompiledMesh& m)
	{
		bounding_volume = m.bounding_volume;
		for (auto& buffer : m.buffers)
			AddBuffer(OpenGLBuffer{ GL_ARRAY_BUFFER, buffer.attribs }.Bind().Buffer(buffer.data.data(), 1, buffer.data.size()));
		_element_array_object = std::move(OpenGLBuffer{ GL_ELEMENT_ARRAY_BUFFER, {} }.Bind().Buffer(m.element_buffer.data(), sizeof(unsigned), m.element_buffer.size()));
		
	}

	OpenGLMesh::OpenGLMesh(OpenGLMesh&& rhs)
		:	_buffers				{ std::move(rhs._buffers) }, 
			_element_array_object	{ std::move(rhs._element_array_object) }, 
			_draw_mode				{ rhs._draw_mode }
	{
	}

	OpenGLMesh& OpenGLMesh::operator=(OpenGLMesh&& rhs)
	{
		std::swap(_buffers,				 rhs._buffers);
		std::swap(_element_array_object, rhs._element_array_object);
		std::swap(_draw_mode, rhs._draw_mode);

		return *this;
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

	void OpenGLMesh::Bind(const renderer_attributes& locations)
	{

		for (GLuint i = 0; i < vtx::Attrib::count; ++i)
			glDisableVertexAttribArray(i);
		for (auto& elem : _buffers)
			elem.BindForDraw(locations);
	}

#pragma warning(disable:4312)

	void OpenGLMesh::Draw()
	{
		_element_array_object.Bind();
		glDrawElements(_draw_mode, _element_array_object.count(), GL_UNSIGNED_INT, 0);
	}

	void OpenGLMesh::DrawInstanced(size_t instances)
	{
		_element_array_object.Bind();
		glDrawElementsInstanced(_draw_mode, _element_array_object.count(), GL_UNSIGNED_INT, 0, static_cast<GLsizei>(instances));
	}

	void OpenGLMesh::BindAndDraw(const renderer_attributes& locations)
	{
		Bind(locations);
		Draw();
	}

	void OpenGLMesh::Reset()
	{
		_buffers.clear();
		_element_array_object = OpenGLBuffer{};
	}

	void OpenGLMesh::AddBuffer(OpenGLBuffer& buf)
	{
		switch (buf.type())
		{
		case GL_ELEMENT_ARRAY_BUFFER:
			_element_array_object = std::move(buf);
			break;
		case GL_ARRAY_BUFFER:
		default:
			_buffers.emplace_back(std::move(buf));
		}
	}

}
