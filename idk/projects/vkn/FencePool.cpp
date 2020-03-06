#include "pch.h"
#include "FencePool.h"
#include <vkn/VulkanState.h>

namespace idk::vkn
{
	FenceObj::~FenceObj()
	{
		if (_src)
		{
			auto ctrl = _ctrl.lock();
			if (ctrl)
			{
				auto guard = (*ctrl).lock();
				_src->Free(std::move(*this));
			}
		}
	}

	FenceObj FencePool::AcquireFence()
	{
		if (_handles.empty())
		{
			GrowFences();
		}
		auto get_handle = [this]() {
			auto guard = _ctrl_block->lock();

			auto index = _handles.back();
			_handles.pop_back();
			return index;
		};
		auto index = get_handle();

		return FenceObj{ *_fences[index],index,this,_ctrl_block };
	}

	void FencePool::Free(FenceObj&& obj)
	{
		_handles.emplace_back(obj.Id());
	}

	FencePool::FencePool()
	{
		_handles.reserve(1000);
		_fences.reserve(1000);
	}

	FencePool::~FencePool()
	{
		if (_handles.size() != _fences.size())
		{
			LOG_ERROR_TO(LogPool::GFX, "Fence pool getting destroyed before FenceObjs are destroyed.");
		}
		_ctrl_block->_destroy_lock=true;
		while (_ctrl_block->_lock > 0);//Wait for everyone who's decrementing to stop.
	}

	size_t FencePool::growth_amount() const
	{
		return std::max(_fences.size(),1ull);
	}

	vk::UniqueFence FencePool::MakeFence()
	{
		return View().Device()->createFenceUnique(vk::FenceCreateInfo{});
	}

	void FencePool::GrowFences()
	{
		auto new_size = _fences.size() + growth_amount();
		_fences.reserve(new_size);
		while (_fences.size() < new_size)
		{
			_handles.emplace_back(_fences.size());
			_fences.emplace_back(MakeFence());
		}
	}

	CmdBufferObj::~CmdBufferObj()
	{
		if (_src)
			_src->Free(std::move(*this));
	}

	CmdBufferObj CmdBufferPool::AcquireCmdBuffer()
	{
		_acquire_lock.Lock();
		if (_handles.empty())
		{
			GrowCmdBuffers();
		}

		auto index = _handles.back();
		_handles.pop_back();
		_acquire_lock.Unlock();
		return CmdBufferObj{ index,this };
	}
//#pragma optimize("",off)
	vk::CommandBuffer CmdBufferObj::operator*() const
	{
		auto cmd_buffer = _src->Get(_id);
		if (!cmd_buffer)
			throw;
		return cmd_buffer;
	}
	CmdBufferPool::CmdBufferPool() :_cmd_pools{}
	{
	}
	void CmdBufferPool::Free(CmdBufferObj&& obj)
	{
		_cmd_buffer[obj.Id()]->reset({});
		_acquire_lock.Lock();
		_handles.emplace_back(obj.Id());
		_acquire_lock.Unlock();
	}

	CmdBufferPool::~CmdBufferPool()
	{
		if (_handles.size() != _cmd_buffer.size())
		{
			LOG_ERROR_TO(LogPool::GFX, "CmdBuffer pool getting destroyed before CmdBufferObjs are destroyed.");
		}
	}

	vk::CommandBuffer CmdBufferPool::Get(size_t id) 
	{
		hlp::SimpleLockGuard guard{ _acquire_lock };
		auto result = *_cmd_buffer[id];
		//_acquire_lock.Unlock();
		return result;
	}

	size_t CmdBufferPool::growth_amount() const
	{
		return std::max(_cmd_buffer.size(), 1ull);
	}


	void CmdBufferPool::GrowCmdBuffers()
	{
		auto growth_amt = growth_amount();
		auto new_size = _cmd_buffer.size() +growth_amt;
		_cmd_buffer.reserve(new_size);
		_cmd_pools.reserve(new_size);
		for (size_t i = 0; i < growth_amt;++i)
		{
			_cmd_pools.emplace_back(View().Device()->createCommandPoolUnique(vk::CommandPoolCreateInfo
				{
					vk::CommandPoolCreateFlagBits::eResetCommandBuffer,*View().QueueFamily().graphics_family
				}));
			auto& _cmd_pool = _cmd_pools.back();
			auto tmp = View().Device()->allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo
				{
					*_cmd_pool,vk::CommandBufferLevel::ePrimary,1
				});
			_handles.emplace_back(_cmd_buffer.size());
			_cmd_buffer.emplace_back(std::move(tmp.back()));
		}

	}
}