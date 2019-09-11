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
		std::shared_ptr<MeshBuffer::Managed> CreateData(string_view data);
	private:
		using offset_t = size_t;
		unique_ptr<Mesh> Create(const hash_table<attrib_index, string_view>& attribs, const vector<uint16_t>&);
		unique_ptr<Mesh> Create(const hash_table<attrib_index, string_view>& attribs, const vector<uint32_t>&);
		void RegisterAttribs(VulkanMesh& mesh,const hash_table<attrib_index, string_view>& attribs);

		unique_ptr<Mesh> Create(const hash_table<attrib_index, std::pair<shared_ptr<MeshBuffer::Managed>, offset_t>>& attribs, const vector<uint16_t>&);
		unique_ptr<Mesh> Create(const hash_table<attrib_index, std::pair<shared_ptr<MeshBuffer::Managed>, offset_t>>& attribs, const vector<uint32_t>&);
		void RegisterAttribs(VulkanMesh& mesh, const hash_table<attrib_index, std::pair<shared_ptr<MeshBuffer::Managed>, offset_t>>& attribs);
		hlp::MemoryAllocator allocator;
	};
}