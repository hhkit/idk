#pragma once

#include <gfx/Mesh.h>
#include <res/ResourceFactory.h>
namespace idk::ogl
{
	class OpenGLMeshFactory
		: public ResourceFactory<Mesh>
	{
		unique_ptr<Mesh> Create() override;
		//unique_ptr<Mesh> Create(const ResourceMeta&);
		unique_ptr<Mesh> Create(string_view filepath) override;
		unique_ptr<Mesh> Create(string_view filepath, const ResourceMeta&) override;
	};
}