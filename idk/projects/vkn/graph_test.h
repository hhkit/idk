#pragma once
#include <idk.h>

namespace idk::vkn
{
	struct GraphicsState;
	struct RenderStateV2;
	class FrameGraph;
}

namespace idk::vkn::gt
{
	struct GraphTest
	{
		GraphTest(FrameGraph&);
		GraphTest();
		GraphTest(const GraphTest&) = delete;
		GraphTest(GraphTest&&);
		GraphTest& operator=(const GraphTest&) = delete;
		GraphTest& operator=(GraphTest&&);
		~GraphTest();
		void DeferredTest(const GraphicsState&,RenderStateV2&);
		void SrgbConversionTest(RenderStateV2&);
	private:
		struct PImpl;
		unique_ptr<PImpl> _pimpl;
	};
}