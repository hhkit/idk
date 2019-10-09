#include "stdafx.h"
#include "Framebuffer.h"
namespace idk
{

	size_t FrameBuffer::NumAttachments() const
	{
		size_t i = 0;
		for (auto& attachment : attachments) 
		{
			i += attachment.size(); 
		}
		return i;
	}
}
