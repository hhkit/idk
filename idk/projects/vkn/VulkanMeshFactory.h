#pragma once
#include <idk.h>
#include <gfx/Mesh.h>
#include <res/ResourceFactory.h>
#include <vkn/MemoryAllocator.h>
#include <vkn/VulkanMesh.h>
namespace idk::vkn
{
	class MeshFactory
		: public ResourceFactory<Mesh>
	{
	public:
		MeshFactory();
		void Init();
		unique_ptr<Mesh> GenerateDefaultResource() override;
		//unique_ptr<Mesh> Create(const ResourceMeta&);
		unique_ptr<Mesh> Create(FileHandle filepath) override;
	private:
		unique_ptr<Mesh> Create(const hash_table<attrib_index, string_view>& attribs, const vector<uint16_t>&, uint32_t num_vertices);
		unique_ptr<Mesh> Create(const hash_table<attrib_index, string_view>& attribs, const vector<uint32_t>&, uint32_t num_vertices);
		void RegisterAttribs(VulkanMesh& mesh,const hash_table<attrib_index, string_view>& attribs);
		hlp::MemoryAllocator allocator;
	};
}