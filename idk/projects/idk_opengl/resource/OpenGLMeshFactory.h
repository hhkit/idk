#pragma once

#include <gfx/Mesh.h>
#include <res/ResourceFactory.h>
namespace idk::ogl
{
	class OpenGLMeshFactory
		: public ResourceFactory<Mesh>
	{
	public:
		unique_ptr<Mesh> GenerateDefaultResource() override;
		unique_ptr<Mesh> Create() override;
		unique_ptr<Mesh> Create(PathHandle filepath) override;
	};
}