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
		vector<OpData> _queued;
		vector<OpData> _results;
		FencePool _load_fences;
		CmdBufferPool _cmd_buffers;
		TextureLoader _loader;
		hlp::MemoryAllocator _allocator;
		dbg::milliseconds time_slice{ 5 };
	};
}

MARK_NON_COPY_CTORABLE(idk::vkn::AsyncTexLoader::OpData)