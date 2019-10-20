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
		eStencil,
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

		RscHandle<Texture> buffer;
		RscHandle<Texture> operator*()const { return buffer; }
		operator RscHandle<Texture>()const { return buffer; }
		const RscHandle<Texture>* operator->()const { return &buffer; }
		virtual ~Attachment() = default;
	};

	class FrameBuffer : public Resource<FrameBuffer>
	{
	public:
		vector<unique_ptr<Attachment>> attachments{};
		unique_ptr<Attachment> depth_attachment{}, stencil_attachment{};
		const Attachment& DepthAttachment()const { return *depth_attachment; }
		const Attachment& Attachment(size_t index)const { return *attachments[index]; }
		size_t NumAttachments()const;
		ivec2 size{};
	};

}