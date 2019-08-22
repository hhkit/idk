#include "pch.h"
#include "OpenGLMeshFactory.h"
#include <idk_opengl/resource/OpenGLMesh.h>

namespace idk::ogl
{
	unique_ptr<Mesh> OpenGLMeshFactory::Create()
	{
		auto retval = std::make_unique<OpenGLMesh>();
		struct Vertex
		{
			vec3 pos;
			vec3 normal;
		};

		vector<OpenGLDescriptor> descriptor
		{
			OpenGLDescriptor{vtx::Attrib::Position, sizeof(Vertex), offsetof(Vertex, pos) },
			OpenGLDescriptor{vtx::Attrib::Normal,   sizeof(Vertex), offsetof(Vertex, normal) }
		};

		vector<Vertex> vertexes
		{
			
		};

		retval->AddBuffer( 
			OpenGLBuffer{ GL_ARRAY_BUFFER, descriptor }
				.Bind()
				.Buffer(vertexes.data(), sizeof(Vertex), s_cast<GLsizei>(vertexes.size()))
		);

		vector<int> indices
		{

		};

		retval->AddBuffer(
			OpenGLBuffer{ GL_ELEMENT_ARRAY_BUFFER, {} }
			.Bind()
			.Buffer(indices.data(), sizeof(int), s_cast<GLsizei>(indices.size()))
		);

		return retval;
	}

	unique_ptr<Mesh> OpenGLMeshFactory::Create(string_view)
	{
		return unique_ptr<Mesh>();
	}

	unique_ptr<Mesh> OpenGLMeshFactory::Create(string_view, const ResourceMeta&)
	{
		return unique_ptr<Mesh>();
	}
}