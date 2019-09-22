#pragma once
#include <idk.h>
#include <gfx/Mesh.h>
#include <res/ResourceFactory.h>
#include <vkn/MemoryAllocator.h>
#include <vkn/VulkanMesh.h>
#include <vkn/VknMeshModder.h>
namespace idk::vkn
{
	class MeshFactory
		: public ResourceFactory<Mesh>
	{
	public:
		MeshFactory();
		unique_ptr<Mesh> GenerateDefaultResource() override;
		//unique_ptr<Mesh> Create(const ResourceMeta&);
		unique_ptr<Mesh> Create(PathHandle filepath) override;
	private:
		MeshModder mesh_modder;
	};
}