#pragma once
#include <idk.h>
#include <vulkan/vulkan.hpp>
#include <parallel/multithread_control.h>

namespace idk::vkn
{

	class CommandCluster
	{
	public:
		using cmd_buffer_t = vk::CommandBuffer;
		CommandCluster(vk::CommandBufferLevel level = vk::CommandBufferLevel::eSecondary);
		cmd_buffer_t GetCommandBuffer();

		void Reset();
	private:
		using thread_id_t = int;
		using cmd_pool_t = vk::UniqueCommandPool;
		using unique_cmd_buffer_t = vk::UniqueCommandBuffer;
		struct Pool
		{
			cmd_pool_t cmd_pool;
			vector<unique_cmd_buffer_t > cmd_buffers{};
			span<unique_cmd_buffer_t > used{};

			Pool(vk::CommandBufferLevel level);


			cmd_buffer_t get_next();
			void reset();
			void grow();
			vk::CommandBufferLevel _buffer_level;
		};

		vk::CommandBufferLevel _buffer_level;
		raynal_rw_lock pool_lock;
		hash_table<thread_id_t, Pool> _pools;

	};


}