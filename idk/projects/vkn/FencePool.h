#pragma once
#include <idk.h>
#include <vulkan/vulkan.hpp>
#include <vkn/VulkanView.h>
#include <vkn/SimpleLock.h>
namespace idk::vkn
{
	class FencePool;

	struct FencePoolCtrlBlock
	{
		struct Guard
		{
			FencePoolCtrlBlock* ptr;
			Guard(FencePoolCtrlBlock* p):ptr{p}
			{
				auto& ctrl = *p;
				if (!ctrl._destroy_lock)
				{
					++ctrl._lock;
					if (ctrl._destroy_lock) //in case the destroy lock was locked before we could increment.
					{
						//Don't continue, the original is destroyed.
						--ctrl._lock;
						ptr = {};
					}
				}
				else
				{
					ptr = {};
				}
			}
			~Guard()
			{
				if (ptr)
				{
					auto& ctrl = *ptr;
					--ctrl._lock;
				}
			}
		};
		Guard lock()
		{
			return Guard{ this };
		}
		std::atomic<int8_t> _lock = 0;
		std::atomic<bool> _destroy_lock = 0;
	};

	struct FenceObj
	{
		using handle_t = size_t;
		FenceObj(vk::Fence fence, handle_t id, FencePool* src, std::weak_ptr<FencePoolCtrlBlock> ctrl) : _fence{ fence }, _id{ id }, _src{ src }, _ctrl{ ctrl }{}
		FenceObj(FenceObj&& rhs) noexcept : _fence{ rhs._fence }, _id{ rhs._id }, _src{ rhs._src }, _ctrl{ std::move(rhs._ctrl) }{rhs._src = nullptr; rhs._src = {}; }
		vk::Fence operator*() const
		{
			return _fence;
		}
		~FenceObj();

		handle_t Id()const { return _id; }
	private:
		vk::Fence _fence;
		handle_t _id;
		FencePool* _src;
		std::weak_ptr<FencePoolCtrlBlock> _ctrl;
	};
	class FencePool
	{
	public:
		FencePool();
		~FencePool();
		FenceObj AcquireFence();
		void Free(FenceObj&& obj);
	private:
		size_t growth_amount()const;
		vk::UniqueFence MakeFence();
		void GrowFences();
		vector<size_t> _handles;
		vector<vk::UniqueFence> _fences;
		std::shared_ptr<FencePoolCtrlBlock> _ctrl_block = std::make_shared<FencePoolCtrlBlock>();
	};

	class CmdBufferPool;
	struct CmdBufferObj
	{
		using handle_t = size_t;
		CmdBufferObj( handle_t id, CmdBufferPool* src) : _id{ id }, _src{ src }{}
		CmdBufferObj(CmdBufferObj&& rhs) noexcept : _id{ rhs._id }, _src{ rhs._src }{rhs._src = nullptr; rhs._src = {}; }
		vk::CommandBuffer operator*() const;
		~CmdBufferObj();

		handle_t Id()const { return _id; }
	private:
		handle_t _id;
		CmdBufferPool* _src;
	};
	class CmdBufferPool
	{
	public:
		CmdBufferObj AcquireCmdBuffer();
		void Free(CmdBufferObj&& obj);
		CmdBufferPool();
		~CmdBufferPool();
		vk::CommandBuffer Get(size_t id)const;
	private:
		size_t growth_amount()const;
		void GrowCmdBuffers();
		vector<size_t> _handles;
		vector < vk::UniqueCommandPool> _cmd_pools;
		vector<vk::UniqueCommandBuffer> _cmd_buffer;
		hlp::SimpleLock _acquire_lock;
	};

}