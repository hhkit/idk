#pragma once
#include "RenderTask.h"
namespace idk::vkn
{
	struct FrameGraphResourceManager;
	namespace FrameGraphDetail
	{
		struct Context : RenderTask
		{
			//TODO put the stuff that needs to be read from here.
			const FrameGraphResourceManager& Resources()const noexcept{ return *resources; }
			FrameGraphResourceManager& Resources() noexcept { return *resources; }
			void SetRscManager(FrameGraphResourceManager& mgr)
			{
				resources = &mgr;
			}
		private:
			FrameGraphResourceManager* resources;
		};

		using Context_t = Context&;
	}
}