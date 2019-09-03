#include "pch.h"
#include "VulkanMeshFactory.h"
#include <vkn/VulkanMesh.h>
#include <vkn/VulkanTypeDescriptors.h>
#include <vkn/VulkanState.h>
#include <vkn/VulkanView.h>
#include <vkn/VulkanWin32GraphicsSystem.h>

#include <core/Core.h>
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


	MeshFactory::MeshFactory():allocator{*Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View().Device(),Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View().PDevice() }
	{
	}

	void MeshFactory::Init()
	{
		//TODO set the uniform layouts (M V P)
		//config.uniform_layouts;

		//TODO Create Pipeline with config
	}

	unique_ptr<Mesh> MeshFactory::GenerateDefaultResource()
	{
		vector<vec3> positions
		{
			vec3{   0, +0.5, 0.5f},
			vec3{-0.5, -0.5, 0.5f},
			vec3{+0.5, -0.5, 0.5f}
		};
		vector<vec3> normals
		{
			vec3{ 0, 0, 1},
			vec3{ 0, 0, 1},
			vec3{ 0, 0, 1}
		};
		vector<uint16_t> indices
		{
			0, 2, 1,
			0, 1, 2,
		};
		/*
		auto& vview = Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View();
		//auto& pdevice = vview.PDevice();
		auto& m_device = vview.Device();
		//auto& dispatcher = vview.Dispatcher();
		auto retval = std::make_unique<VulkanMesh>();


		auto num_vtx_bytes = hlp::buffer_size(positions);
		//hlp::MemoryAllocator allocator{};
		//TODO replace with memory allocator for mesh
		auto&& [pbuffer, palloc] = hlp::CreateAllocBindBuffer(vview.PDevice(), *vview.Device(), num_vtx_bytes, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal,allocator, vview.Dispatcher());
		auto&& [nbuffer,nalloc]  = hlp::CreateAllocBindBuffer(vview.PDevice(), *vview.Device(), num_vtx_bytes, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal,allocator, vview.Dispatcher());
		//Make the buffer


		auto&& [idx_buffer, idx_alloc] = hlp::CreateAllocBindBuffer(vview.PDevice(), *vview.Device(), hlp::buffer_size(indices), vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal,allocator, vview.Dispatcher());

		hlp::TransferData(*vview.Commandpool(), vview.GraphicsQueue(), vview.PDevice(), *m_device, 0, num_vtx_bytes, std::data(positions), *pbuffer);
		hlp::TransferData(*vview.Commandpool(), vview.GraphicsQueue(), vview.PDevice(), *m_device, 0, num_vtx_bytes, std::data(normals), *nbuffer);
		hlp::TransferData(*vview.Commandpool(), vview.GraphicsQueue(), vview.PDevice(), *m_device, 0, hlp::buffer_size(indices), std::data(indices), *idx_buffer);

		retval->SetBuffer(attrib_index::Position, MeshBuffer{ std::move(pbuffer),std::move(palloc),num_vtx_bytes });
		retval->SetBuffer(attrib_index::Normal, MeshBuffer{ std::move(nbuffer),std::move(nalloc),num_vtx_bytes });
		retval->SetIndexBuffer(
			MeshBuffer{ std::move(idx_buffer),std::move(idx_alloc) , hlp::buffer_size(indices) },
			s_cast<uint32_t>(indices.size()),
			vk::IndexType::eUint16
		);
		*/
		hash_table<attrib_index, string_view> attribs;
		{
			auto& buffer = positions;
			attribs[attrib_index::Position] = string_view{ r_cast<const char*>(std::data(buffer)),hlp::buffer_size(buffer) };
		}
		{
			auto& buffer = normals;
			attribs[attrib_index::Normal] = string_view{ r_cast<const char*>(std::data(buffer)),hlp::buffer_size(buffer) };
		}



		return Create(attribs,indices,s_cast<uint32_t>(positions.size()));
	}

	unique_ptr<Mesh> idk::vkn::MeshFactory::Create(FileHandle filepath)
	{
		return unique_ptr<Mesh>();
	}

	unique_ptr<Mesh> MeshFactory::Create(const hash_table<attrib_index, string_view>& attribs, const vector<uint16_t>& index_buffer,uint32_t num_vertices)
	{
		auto& vview = Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View();
		//auto& pdevice = vview.PDevice();
		auto& m_device = vview.Device();
		//auto& dispatcher = vview.Dispatcher();
		auto retval = std::make_unique<VulkanMesh>();
		RegisterAttribs(*retval, attribs);
		{
			auto num_vtx_bytes = hlp::buffer_size(index_buffer);
			auto data = std::data(index_buffer);
			auto&& [pbuffer, palloc] = hlp::CreateAllocBindBuffer(vview.PDevice(), *vview.Device(), num_vtx_bytes, vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal, allocator, vview.Dispatcher());
			hlp::TransferData(*vview.Commandpool(), vview.GraphicsQueue(), vview.PDevice(), *m_device, 0, num_vtx_bytes, data, *pbuffer);
			retval->SetIndexBuffer(
				MeshBuffer{ std::move(pbuffer),std::move(palloc) , num_vtx_bytes },
				s_cast<uint32_t>(index_buffer.size()),
				vk::IndexType::eUint16
			);
		}
		return retval;
	}
	unique_ptr<Mesh> MeshFactory::Create(const hash_table<attrib_index, string_view>& attribs, const vector<uint32_t>& index_buffer, uint32_t num_vertices)
	{
		auto& vview = Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View();
		//auto& pdevice = vview.PDevice();
		auto& m_device = vview.Device();
		//auto& dispatcher = vview.Dispatcher();
		auto retval = std::make_unique<VulkanMesh>();
		RegisterAttribs(*retval, attribs);
		{
			auto num_vtx_bytes = hlp::buffer_size(index_buffer);
			auto data = std::data(index_buffer);
			auto&& [pbuffer, palloc] = hlp::CreateAllocBindBuffer(vview.PDevice(), *vview.Device(), num_vtx_bytes, vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal, allocator, vview.Dispatcher());
			hlp::TransferData(*vview.Commandpool(), vview.GraphicsQueue(), vview.PDevice(), *m_device, 0, num_vtx_bytes, data, *pbuffer);
			retval->SetIndexBuffer(
				MeshBuffer{ std::move(pbuffer),std::move(palloc) , num_vtx_bytes },
				s_cast<uint32_t>(index_buffer.size()),
				vk::IndexType::eUint32 
			);
		}
		return retval;
	}

	void MeshFactory::RegisterAttribs(VulkanMesh& mesh, const hash_table<attrib_index, string_view>& attribs)
	{
		auto& vview = Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View();
		//auto& pdevice = vview.PDevice();
		auto& m_device = vview.Device();
		for (auto& attrib : attribs)
		{
			auto num_vtx_bytes = hlp::buffer_size(attrib.second);
			auto data = std::data(attrib.second);
			auto&& [pbuffer, palloc] = hlp::CreateAllocBindBuffer(vview.PDevice(), *vview.Device(), num_vtx_bytes, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal, allocator, vview.Dispatcher());
			hlp::TransferData(*vview.Commandpool(), vview.GraphicsQueue(), vview.PDevice(), *m_device, 0, num_vtx_bytes, data, *pbuffer);
			mesh.SetBuffer(attrib.first, MeshBuffer{ std::move(pbuffer),std::move(palloc),num_vtx_bytes });
		}
	}

}