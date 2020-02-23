#pragma once
#include "RenderTask.h"
#include <meta/stl_hack.h>

namespace idk::vkn
{
	class FrameGraphResourceManager;
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
MARK_NON_COPY_CTORABLE(idk::vkn::FrameGraphDetail::Context);