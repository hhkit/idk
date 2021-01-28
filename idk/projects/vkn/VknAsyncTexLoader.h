#pragma once
#include <idk.h>
#include <vkn/VknTextureView.h>
#include <vkn/ManagedVulkanObjs.h>
#include <parallel/ThreadPool.h>
#include <res/ResourceHandle.h>

#include <core/Core.inl>
#include <res/ResourceHandle.inl>
#include <vkn/VknTexture.h>
#include <vkn/AsyncTexLoadInfo.h>

#include <vkn/FencePool.h>
#include <vkn/VknTextureLoader.h>
#include <vkn/VknTextureData.h>

#include <vkn/Stopwatch.h>

#include <meta/stl_hack.h>


namespace idk::vkn
{
	struct AsyncTexLoader
	{
	public:
		void Load(AsyncTexLoadInfo&& load_info, VknTexture& tex,RscHandle<VknTexture> tex_handle);
		void UpdateTextures();

		void ClearQueue();

		size_t num_pending()const noexcept;

		struct DbgState
		{
			bool load1=false;
			bool load2=false;
			bool wait_results = false;
			bool wait_results2 = false;
			bool has_future_before_update = false;
			bool has_future_after_update = false;
			std::string future_err;
			size_t pending_scratch_queue=0;
			size_t pending_queue=0;
		};

		DbgState state;

		struct ExecProxy
		{
			AsyncTexLoader* ptr;
			void exec();
		};
	private:
		struct OpData
		{
			mt::Future<void> future;
			RscHandle<VknTexture> handle;
			AsyncTexLoadInfo info;
			std::unique_ptr<VknTextureData> data;
		};


		size_t erase_result(size_t i);

		void ProcessFrame();

		std::optional<mt::Future<void>> ready;
		hlp::MemoryAllocator _allocator;
		FencePool _load_fences;
		CmdBufferPool _cmd_buffers;
		std::mutex _queue_lock;
		vector<OpData> _queued;
		vector<OpData> _process_queue;
		vector<OpData> _results;
		TextureLoader _loader;
		dbg::milliseconds time_slice{ 10 };
	};
}

MARK_NON_COPY_CTORABLE(idk::vkn::AsyncTexLoader::OpData)