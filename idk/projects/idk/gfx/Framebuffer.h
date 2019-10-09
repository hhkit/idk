#pragma once
#include <stdafx.h>
namespace idk
{
	enum class StoreOp
	{
		eDontCare,
		eStore
	};
	enum class LoadOp
	{
		eDontCare,
		eClear,
		eLoad
	};
	enum AttachmentType
	{
		eColor,
		eDepth,
		eSizeAT
	};

	inline AttachmentType& operator++(AttachmentType& type)
	{
		type = s_cast<AttachmentType>(s_cast<int>(type) + 1);
		return type;
	}
	struct Attachment
	{
		LoadOp  load_op{};
		StoreOp store_op{};
		LoadOp  stencil_load_op{};
		StoreOp stencil_store_op{};
		virtual ~Attachment() = default;
	};

	class FrameBuffer : public Resource<FrameBuffer>
	{
	public:
		vector<unique_ptr<Attachment>> attachments[AttachmentType::eSizeAT];
		size_t NumAttachments()const;
		ivec2 size;
	};

}