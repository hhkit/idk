#include "pch.h"
#include "VulkanMeshFactory.h"
#include <vkn/VulkanMesh.h>
#include <vkn/VulkanTypeDescriptors.h>
#include <vkn/VulkanState.h>
#include <vkn/VulkanView.h>
#include <vkn/VulkanWin32GraphicsSystem.h>
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

	unique_ptr<Mesh> MeshFactory::Create()
	{
		auto& vview = Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View();
		auto& pdevice = vview.PDevice();
		auto& m_device = vview.Device();
		auto& dispatcher = vview.Dispatcher();
		auto retval = std::make_unique<VulkanMesh>();
		struct Vertex
		{
			vec3 pos;
			vec3 normal;
		};

		vector<VulkanDescriptor> descriptor
		{
			VulkanDescriptor{vtx::Attrib::Position, sizeof(Vertex), offsetof(Vertex, pos) },
			VulkanDescriptor{vtx::Attrib::Normal,   sizeof(Vertex), offsetof(Vertex, normal) }
		};

		vector<Vertex> vertices
		{
			Vertex{vec3{0,    +0.5, 0}},
			Vertex{vec3{-0.5, -0.5, 0}},
			Vertex{vec3{+0.5, -0.5, 0}}
		};
		auto num_vtx_bytes = hlp::buffer_size(vertices);
		auto&& [buffer,memory]=hlp::CreateAllocBindBuffer(vview.PDevice(), *vview.Device(), num_vtx_bytes, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal, vview.Dispatcher());
		//Make the buffer
		retval->SetBuffer(BufferType::eAttrib, MeshBuffer{std::move(buffer),num_vtx_bytes});

		vector<int> indices
		{
			0, 2, 1
		};

		auto&& [idx_buffer, idx_memory] = hlp::CreateAllocBindBuffer(vview.PDevice(), *vview.Device(), hlp::buffer_size(indices), vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal, vview.Dispatcher());
		retval->SetBuffer(
			BufferType::eIndex, MeshBuffer{ std::move(idx_buffer) , hlp::buffer_size(indices)}
		);
		hlp::TransferData(*vview.Commandpool(), vview.GraphicsQueue(), vview.PDevice(), *m_device, 0, num_vtx_bytes, std::data(vertices), *buffer);
		hlp::TransferData(*vview.Commandpool(), vview.GraphicsQueue(), vview.PDevice(), *m_device, 0, hlp::buffer_size(indices), std::data(indices), *idx_buffer);
		return retval;
	}

	unique_ptr<Mesh> idk::vkn::MeshFactory::Create(FileHandle filepath)
	{
		return unique_ptr<Mesh>();
	}

}