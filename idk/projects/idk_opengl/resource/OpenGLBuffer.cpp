#include "pch.h"
#include "OpenGLBuffer.h"

namespace idk::ogl
{
	OpenGLBuffer::OpenGLBuffer(GLuint type, const vector<OpenGLDescriptor>& desc)
		: _type{ type }, descriptor{ desc }
	{
		glGenBuffers(1, &_id);
	}

	OpenGLBuffer::OpenGLBuffer(OpenGLBuffer&& buf)
		: _type{ buf._type }, _id{ buf._id }, descriptor{ std::move(buf.descriptor) }
	{
		buf._type = 0;
		buf._id = 0;
	}

	OpenGLBuffer::~OpenGLBuffer()
	{
		glDeleteBuffers(1, &_id);
	}

	GLuint OpenGLBuffer::type() const
	{
		return _type;
	}

	GLsizei OpenGLBuffer::count() const
	{
		return _size;
	}

	OpenGLBuffer& OpenGLBuffer::Bind()
	{
		glBindBuffer(_type, _id);
		return *this;
	}

	OpenGLBuffer& OpenGLBuffer::BindForDraw(const renderer_reqs& locations)
	{

#pragma warning(disable:4312)

		for (auto& elem : descriptor)
		{
			auto find_type = OpenGLAttribs.find(elem.attrib);
			assert(find_type != OpenGLAttribs.end());

			auto loc = locations.requirements.find(elem.attrib)->second;
			glVertexAttribPointer(
				loc,
				find_type->second.size,
				find_type->second.type,
				GL_FALSE,
				elem.stride,
				r_cast<void*>(elem.offset)
			);

			glEnableVertexAttribArray(loc);
		}
		return *this;
	}

	OpenGLBuffer& OpenGLBuffer::Buffer(void* data, size_t stride, GLsizei count)
	{
		_size = count;
		glBufferData(_type, count * stride, data, GL_STATIC_READ);
		return *this;
	}

	OpenGLBuffer& OpenGLBuffer::operator=(OpenGLBuffer&& buf)
	{
		std::swap(_type, buf._type);
		std::swap(_id, buf._id);
		std::swap(descriptor, buf.descriptor);
		return *this;
	}
}