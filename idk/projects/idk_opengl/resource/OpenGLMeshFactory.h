#pragma once

#include <gfx/Mesh.h>
#include <res/ResourceFactory.h>
namespace idk::ogl
{
	class OpenGLMeshFactory
		: public ResourceFactory<Mesh>
	{
	public:
		unique_ptr<Mesh> DefaultResource() override;
		unique_ptr<Mesh> Create() override;
		unique_ptr<Mesh> Create(FileHandle filepath) override;
	};
}