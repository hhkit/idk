#include "pch.h"
#include "FencePool.h"
#include <vkn/VulkanState.h>

namespace idk::vkn
{
	FenceObj::~FenceObj()
	{
		if (_src)
			_src->Free(std::move(*this));
	}

	FenceObj FencePool::AcquireFence()
	{
		if (_handles.empty())
		{
			GrowFences();
		}

		auto index = _handles.back();
		_handles.pop_back();
		return FenceObj{ *_fences[index],index,this };
	}

	void FencePool::Free(FenceObj&& obj)
	{
		_handles.emplace_back(obj.Id());
	}

	FencePool::~FencePool()
	{
		if (_handles.size() != _fences.size())
		{
			LOG_ERROR_TO(LogPool::GFX, "Fence pool getting destroyed before FenceObjs are destroyed.");
		}
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
		if (_handles.empty())
		{
			GrowCmdBuffers();
		}

		auto index = _handles.back();
		_handles.pop_back();
		return CmdBufferObj{ *_CmdBuffers[index],index,this };
	}

	vk::CommandBuffer CmdBufferObj::operator*() const
	{
		return _src->Get(_id);
	}
	CmdBufferPool::CmdBufferPool() :_CmdPool{View().Device()->createCommandPoolUnique(vk::CommandPoolCreateInfo
			{
				vk::CommandPoolCreateFlagBits::eResetCommandBuffer,*View().QueueFamily().graphics_family
			})}
	{
	}
	void CmdBufferPool::Free(CmdBufferObj&& obj)
	{
		_CmdBuffers[obj.Id()]->reset({});
		_handles.emplace_back(obj.Id());
	}

	CmdBufferPool::~CmdBufferPool()
	{
		if (_handles.size() != _CmdBuffers.size())
		{
			LOG_ERROR_TO(LogPool::GFX, "CmdBuffer pool getting destroyed before CmdBufferObjs are destroyed.");
		}
	}

	vk::CommandBuffer idk::vkn::CmdBufferPool::Get(size_t id) const
	{
		return *_CmdBuffers[id];
	}

	size_t CmdBufferPool::growth_amount() const
	{
		return std::max(_CmdBuffers.size(), 1ull);
	}


	void CmdBufferPool::GrowCmdBuffers()
	{
		auto growth_amt = growth_amount();
		auto new_size = _CmdBuffers.size() +growth_amt;
		_CmdBuffers.reserve(new_size);
		auto tmp = View().Device()->allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo
			{
				*_CmdPool,vk::CommandBufferLevel::ePrimary,static_cast<uint32_t>(growth_amt)
			});
		for (auto& cmd_buffer : tmp)
		{
			_handles.emplace_back(_CmdBuffers.size());
			_CmdBuffers.emplace_back(std::move(cmd_buffer));
		}
	}
}