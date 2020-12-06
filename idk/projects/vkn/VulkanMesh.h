#pragma once
#include <idk.h>
#include <gfx/Mesh.h>
#include <vulkan/vulkan.hpp>
#include <gfx/vertex_descriptor.h>
#include <vkn/MemoryAllocator.h>

#include <vkn/VulkanResourceManager.h>
#include <editorstatic\imgui\imgui.h>
namespace idk
{
	struct CompiledMesh;
}

namespace idk::vkn
{
	using attrib_index = vtx::Attrib;
	struct MeshBuffer
	{
		struct Managed
		{
			//vk::UniqueBuffer 
			VulkanRsc<vk::Buffer> buffer{};
			hlp::UniqueAlloc alloc{};
			size_t len{};
			Managed() = default;
			Managed(
				vk::UniqueBuffer b,
			hlp::UniqueAlloc a,
			size_t l):buffer{std::move(b)},alloc{ std::move(a) },len{l}{}
		};
		std::shared_ptr<Managed> memory;
		std::optional<vk::Buffer> buffer()const { return (memory) ? std::optional<vk::Buffer> {*memory->buffer} : std::nullopt; }
		size_t offset{};
	};
	class VulkanMesh : public Mesh
	{
	public:
		using buffers_t = hash_table<attrib_index, MeshBuffer>;
		VulkanMesh() = default;
		VulkanMesh(CompiledMesh&& m);

		VulkanMesh(VulkanMesh&& m) = default;
		VulkanMesh& operator=(VulkanMesh&& m) = default;


		VulkanMesh(const VulkanMesh& m) = delete;
		VulkanMesh& operator=(const VulkanMesh& m) = delete;



		const MeshBuffer& Get(attrib_index index)const;
		bool Has(attrib_index index)const;
		const buffers_t& Buffers()const;
		int GetAttribs() const override;
		const std::optional<MeshBuffer>& GetIndexBuffer()const;
		uint32_t IndexCount()const { return index_count; }
		void SetIndexBuffer(MeshBuffer&& buffer, uint32_t count, vk::IndexType type);
		void SetBuffer(attrib_index type, MeshBuffer&& buffer);
		vk::IndexType IndexType()const { return index_type; }

		//Set to false when done loading. Defaults to true.
		void use_default(bool value);

	private:
		buffers_t buffers{};
		vk::IndexType index_type = vk::IndexType::eUint16;
		uint32_t index_count{};
		std::optional<MeshBuffer> index_buffer;
		bool use_default_=false;
	};

}
