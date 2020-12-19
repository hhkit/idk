#pragma once
#include <idk.h>
#include <gfx/Mesh.h>
#include <gfx/MeshFactory.h>
#include <res/EasyFactory.h>
#include <vkn/MemoryAllocator.h>
#include <vkn/VulkanMesh.h>
#include <vkn/VknMeshModder.h>
#include <vkn/VulkanMeshAsyncLoader.h>
namespace idk::vkn
{
	class MeshFactory
		: public IMeshFactory
	{
	public:
		MeshFactory();
		void GenerateDefaultMeshes()override;
		unique_ptr<Mesh> GenerateDefaultResource() override;
		unique_ptr<Mesh> Create()override;
		MeshModder mesh_modder;
		std::shared_ptr<VulkanMeshAsyncLoader> async_loader = std::make_shared<VulkanMeshAsyncLoader>();
	private:
	};
}