#pragma once
#include "AsyncLoader.h"
#include "gfx/CompiledMesh.h"
#include "VulkanMesh.h"
namespace idk::vkn
{
	struct MeshLoaderEntry
	{
		CompiledMesh compiled_mesh;
		RscHandle<VulkanMesh> handle;
	};
	class VulkanMeshAsyncLoader : public AsyncLoader<MeshLoaderEntry>
	{
	public:
		void ProcessEntry(Entry& entry)override;
	};
}