#include "pch.h"
#include "CommandCluster.h"

#include <vkn/VulkanView.h>
#include <vkn/VulkanState.inl>
#include <parallel/ThreadPool.h>
namespace idk::vkn
{
	CommandCluster::CommandCluster(vk::CommandBufferLevel level):_buffer_level{level}
	{
	}
	CommandCluster::cmd_buffer_t CommandCluster::GetCommandBuffer()
{
	//acquire pools
	int thd_id = mt::thread_id();
	{
		auto guard = pool_lock.lock_read();
		auto itr = _pools.find(thd_id);
		if (_pools.end() != itr)
		{
			return itr->second.get_next();
		}
	}
	auto guard = pool_lock.lock_write();
	return _pools.emplace(thd_id,_buffer_level).first->second.get_next();
}

void CommandCluster::Reset()
{
	for (auto& [id, pool] : _pools)
	{
		pool.reset();
	}
}

CommandCluster::Pool::Pool(vk::CommandBufferLevel level):_buffer_level{level}
{
	auto device = *View().Device();
	;
	cmd_pool = device.createCommandPoolUnique(vk::CommandPoolCreateInfo
		{
			vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
			*View().QueueFamily().graphics_family
		});// , nullptr, View().Dispatcher());
}

CommandCluster::cmd_buffer_t CommandCluster::Pool::get_next()
{
	if (used.size() == cmd_buffers.size())
	{
		grow();
	}
	auto next = used._end++;
	return **next;
}

void CommandCluster::Pool::reset()
{
	auto device = *View().Device();
	device.resetCommandPool(*cmd_pool, vk::CommandPoolResetFlags{});
	used = span{ cmd_buffers.data(),cmd_buffers.data() };
}

void CommandCluster::Pool::grow()
{
	auto device = *View().Device();
	cmd_buffers.resize(std::max(1ui64, cmd_buffers.size() * 2));
	used = span{ cmd_buffers.data(),cmd_buffers.data() + used.size() };
	span new_space = { used.end(),used.end() + cmd_buffers.size() - used.size() };
	auto create_info = vk::CommandBufferAllocateInfo{ *cmd_pool,_buffer_level , static_cast<uint32_t>(new_space.size()) };
	auto new_buffers = device.allocateCommandBuffersUnique(create_info);
	std::move(new_buffers.begin(), new_buffers.end(), new_space.begin());
}

}