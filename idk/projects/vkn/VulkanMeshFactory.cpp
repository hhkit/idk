#include "pch.h"
#include "VulkanMeshFactory.h"
#include <vkn/VulkanMesh.h>
#include <vkn/VulkanTypeDescriptors.h>
#include <vkn/VulkanState.h>
#include <vkn/VulkanView.h>
#include <vkn/VulkanWin32GraphicsSystem.h>

#include <vkn/MemoryAllocator.h>


namespace idk::vkn
{
	namespace hlp
	{

		vk::UniqueCommandBuffer CreateCommandBuffer(vk::CommandPool command_pool, vk::Device device, vk::CommandBufferLevel cmd_level= vk::CommandBufferLevel::ePrimary)
		{
				//For RenderState
				//rss.resize(max_frames_in_flight);
			vk::CommandBufferAllocateInfo rs_alloc_info
			{
				command_pool
				,cmd_level
				,1
			};
			auto cmd_buffers = device.allocateCommandBuffersUnique(rs_alloc_info, vk::DispatchLoaderDefault{});
			return std::move(cmd_buffers[0]);
		}
		//Expensive (probably).
		void TransferData(vk::CommandPool cmd_pool, vk::Queue queue, vk::PhysicalDevice pdevice, vk::Device device, size_t dst_offset, size_t num_bytes, const void* data, vk::Buffer dst_buffer)
		{
			auto tmp_cmd_buffer = hlp::CreateCommandBuffer(cmd_pool, device);
			auto dispatcher = vk::DispatchLoaderDefault{};

			vk::DeviceSize bufferSize = num_bytes;

			auto [stagingBuffer, stagingBufferMemory] = hlp::CreateAllocBindBuffer(
				pdevice, device, bufferSize,
				vk::BufferUsageFlagBits::eTransferSrc,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
				dispatcher);

			{
				hlp::MapMemory(device, *stagingBufferMemory, dst_offset, data, bufferSize, dispatcher);
			}
			//m_vertex_buffers.emplace_back( std::move(instance_buffer));
			//m_vertex_memories.emplace_back(std::move(instance_memory));

			hlp::CopyBuffer(*tmp_cmd_buffer, queue, *stagingBuffer, dst_buffer, bufferSize);
		}

	}

	struct MeshVtx
	{
		alignas(sizeof(vec4)) vec3 pos;
		alignas(sizeof(vec4)) vec3 nml;
	};

	template<typename CItr>
	void CpyWithStride(CItr src_start, CItr src_end, void* dst_start, void* dst_end, uint32_t dst_stride)
	{
		using T = decltype(*src_start);
		uint8_t* d_st = r_cast<uint8_t*>(dst_start);
		uint8_t* d_ed = r_cast<uint8_t*>(dst_end);
		while (src_start != src_end && dst_end >dst_start)
		{
			memcpy_s(d_st, d_ed - d_st, src_start, sizeof(T));
			d_st += dst_stride;
			++src_start;
		}
	}


	void MeshFactory::Init()
	{
		//TODO set the uniform layouts (M V P)
		//config.uniform_layouts;

		//TODO Create Pipeline with config
	}

	unique_ptr<Mesh> MeshFactory::Create()
	{

		auto& vview = Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View();
		auto& pdevice = vview.PDevice();
		auto& m_device = vview.Device();
		auto& dispatcher = vview.Dispatcher();
		auto retval = std::make_unique<VulkanMesh>();

		vector<MeshVtx> vertices
		{
			MeshVtx{vec3{0,    +0.5, 0}},
			MeshVtx{vec3{-0.5, -0.5, 0}},
			MeshVtx{vec3{+0.5, -0.5, 0}}
		};



		vector<vec3> positions
		{
			vec3{   0, +0.5, 0},
			vec3{-0.5, -0.5, 0},
			vec3{+0.5, -0.5, 0}
		};
		vector<vec3> normals
		{
			vec3{ 0, 0, 1},
			vec3{ 0, 0, 1},
			vec3{ 0, 0, 1}
		};

		auto num_vtx_bytes = hlp::buffer_size(positions);
		hlp::MemoryAllocator allocator{};
		//TODO replace with memory allocator for mesh
		auto&& [pbuffer, palloc] = hlp::CreateAllocBindBuffer(vview.PDevice(), *vview.Device(), num_vtx_bytes, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal,allocator, vview.Dispatcher());
		auto&& [nbuffer,nalloc]  = hlp::CreateAllocBindBuffer(vview.PDevice(), *vview.Device(), num_vtx_bytes, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal,allocator, vview.Dispatcher());
		//Make the buffer
		retval->SetBuffer(attrib_index::Position, MeshBuffer{ std::move(pbuffer),std::move(palloc),num_vtx_bytes });
		retval->SetBuffer(attrib_index::Normal,   MeshBuffer{ std::move(nbuffer),std::move(nalloc),num_vtx_bytes});

		vector<uint16_t> indices
		{
			0, 2, 1
		};

		auto&& [idx_buffer, idx_alloc] = hlp::CreateAllocBindBuffer(vview.PDevice(), *vview.Device(), hlp::buffer_size(indices), vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal,allocator, vview.Dispatcher());
		retval->SetIndexBuffer(
			MeshBuffer{ std::move(idx_buffer),std::move(idx_alloc) , hlp::buffer_size(indices) },
			s_cast<uint32_t>(indices.size())
		);
		hlp::TransferData(*vview.Commandpool(), vview.GraphicsQueue(), vview.PDevice(), *m_device, 0, num_vtx_bytes, std::data(positions), *pbuffer);
		hlp::TransferData(*vview.Commandpool(), vview.GraphicsQueue(), vview.PDevice(), *m_device, 0, num_vtx_bytes, std::data(normals), *nbuffer);
		hlp::TransferData(*vview.Commandpool(), vview.GraphicsQueue(), vview.PDevice(), *m_device, 0, hlp::buffer_size(indices), std::data(indices), *idx_buffer);
		return retval;
	}

	unique_ptr<Mesh> idk::vkn::MeshFactory::Create(FileHandle filepath)
	{
		return unique_ptr<Mesh>();
	}

}