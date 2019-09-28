#include "stdafx.h"
#include "RenderTarget.h"

namespace idk
{
	float RenderTarget::AspectRatio() const
	{
		return s_cast<float>(meta.size.x) / meta.size.y;
	}
	RscHandle<Texture> RenderTarget::GetAttachment(AttachmentType type, size_t index)const
	{
		return GetMeta().textures[attachments[type][index]];
	}
	size_t  RenderTarget::AddAttachment(AttachmentType type, uint32_t size_x, uint32_t size_y)
	{
		RscHandle<Texture> texture = Core::GetResourceManager().Create<Texture>();
		texture->Size(ivec2{size_x,size_y});
		AddAttachmentImpl(type, texture);
		auto new_index = meta.textures.size();
		meta.textures.emplace_back(texture);
		attachments[type].emplace_back(new_index);
		return new_index;
	}
	size_t RenderTarget::NumAttachments(AttachmentType type)const
	{
		return attachments[type].size();
	}
}
