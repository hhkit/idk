#pragma once

#include <gfx/Mesh.h>
#include <res/ResourceFactory.h>
namespace idk::vkn
{
	class MeshFactory
		: public ResourceFactory<Mesh>
	{
	public:
		unique_ptr<Mesh> Create() override;
		//unique_ptr<Mesh> Create(const ResourceMeta&);
		unique_ptr<Mesh> Create(FileHandle filepath) override;
	};
}