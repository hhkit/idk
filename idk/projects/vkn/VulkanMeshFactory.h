#pragma once
#include <idk.h>
#include <gfx/Mesh.h>
#include <gfx/MeshFactory.h>
#include <res/EasyFactory.h>
#include <vkn/MemoryAllocator.h>
#include <vkn/VulkanMesh.h>
#include <vkn/VknMeshModder.h>
namespace idk::vkn
{
	class MeshFactory
		: public virtual IMeshFactory
	{
	public:
		MeshFactory();
		void GenerateDefaultMeshes()override;
		unique_ptr<Mesh> GenerateDefaultResource() override;
		unique_ptr<Mesh> Create()override;
	private:
		MeshModder mesh_modder;
	};
}