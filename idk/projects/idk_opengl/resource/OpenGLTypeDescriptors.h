#pragma once
#include <idk.h>
#include <glad/glad.h>
#include <gfx/vertex_descriptor.h>

namespace idk::ogl
{
	struct OpenGLType
	{
		GLenum type;
		GLint size;
	};

	const static inline hash_table<vtx::Attrib, OpenGLType> OpenGLAttribs
	{
		std::make_pair(vtx::Attrib::Position,   OpenGLType{ GL_FLOAT, 3}),
		std::make_pair(vtx::Attrib::Normal,     OpenGLType{ GL_FLOAT, 3}),
		std::make_pair(vtx::Attrib::Tangent,    OpenGLType{ GL_FLOAT, 3}),
		std::make_pair(vtx::Attrib::UV,         OpenGLType{ GL_FLOAT, 2}),
		std::make_pair(vtx::Attrib::BoneID,     OpenGLType{ GL_INT,   4}),
		std::make_pair(vtx::Attrib::BoneWeight, OpenGLType{ GL_FLOAT, 4}),
	};

	struct OpenGLDescriptor
	{
		vtx::Attrib attrib = vtx::Attrib::Position;
		unsigned    offset = 0;
		unsigned    stride = 0;
	};
}