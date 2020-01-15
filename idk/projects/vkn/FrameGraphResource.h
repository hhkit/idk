#pragma once
namespace idk::vkn
{
	using fgr_id = size_t;
	struct FrameGraphResource
	{
		fgr_id id;
		bool readonly = true;
	};
	inline bool operator==(const FrameGraphResource& lhs, const FrameGraphResource& rhs)noexcept
	{
		return lhs.id == rhs.id && lhs.readonly == rhs.readonly;
	}
	using FrameGraphResourceReadOnly = FrameGraphResource;
	using FrameGraphResourceMutable = FrameGraphResource;
}