#pragma once
namespace idk::vkn
{
	using fgr_id = size_t;
	struct FrameGraphResource
	{
		fgr_id id;
		bool readonly = true;
	};
	using FrameGraphResourceReadOnly = FrameGraphResource;
	using FrameGraphResourceMutable = FrameGraphResource;
}