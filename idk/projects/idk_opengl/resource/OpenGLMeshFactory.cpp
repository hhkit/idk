#include "pch.h"
#include "OpenGLMeshFactory.h"
#include <idk_opengl/resource/OpenGLMesh.h>

namespace idk::ogl
{
	unique_ptr<Mesh> OpenGLMeshFactory::GenerateDefaultResource()
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
			Vertex{vec3{0,    +0.5, 0}},
			Vertex{vec3{-0.5, -0.5, 0}},
			Vertex{vec3{+0.5, -0.5, 0}}
		};
		retval->AddMeshEntry(0, 0, 3, 0);
		retval->AddBuffer( 
			OpenGLBuffer{ GL_ARRAY_BUFFER, descriptor }
				.Bind()
				.Buffer(vertexes.data(), sizeof(Vertex), s_cast<GLsizei>(vertexes.size()))
		);

		vector<int> indices
		{
			0, 2, 1
		};

		retval->AddBuffer(
			OpenGLBuffer{ GL_ELEMENT_ARRAY_BUFFER, {} }
			.Bind()
			.Buffer(indices.data(), sizeof(int), s_cast<GLsizei>(indices.size()))
		);

		return retval;
	}

	unique_ptr<Mesh> OpenGLMeshFactory::Create()
	{
		return std::make_unique<OpenGLMesh>();
	}

	unique_ptr<Mesh> OpenGLMeshFactory::Create(PathHandle)
	{
		return unique_ptr<Mesh>();
	}
}