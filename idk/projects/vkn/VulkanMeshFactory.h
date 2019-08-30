#pragma once

#include <gfx/Mesh.h>
#include <res/ResourceFactory.h>
#include <vkn/MemoryAllocator.h>
namespace idk::vkn
{
	class MeshFactory
		: public ResourceFactory<Mesh>
	{
	public:
		MeshFactory();
		void Init();
		unique_ptr<Mesh> Create() override;
		//unique_ptr<Mesh> Create(const ResourceMeta&);
		unique_ptr<Mesh> Create(FileHandle filepath) override;
	private:
		hlp::MemoryAllocator allocator;
	};
}