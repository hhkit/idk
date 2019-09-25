#pragma once

#include <gfx/Mesh.h>
#include <gfx/MeshFactory.h>
#include <res/ResourceFactory.h>
namespace idk::ogl
{
	class OpenGLMeshFactory
		: public IMeshFactory
	{
	public:
		void GenerateDefaultMeshes() override;
		unique_ptr<Mesh> GenerateDefaultResource() override;
		unique_ptr<Mesh> Create() override;
	};
}