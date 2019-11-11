#include "stdafx.h"
#include "Framebuffer.h"
namespace idk
{

	size_t FrameBuffer::NumAttachments() const
	{
		size_t i = attachments.size();
		i = (depth_attachment) ? i + 1 : i;
		i = (stencil_attachment) ? i + 1 : i;//Probably gotta merge depth and stencil
		return i;
	}
	size_t FrameBuffer::NumLayers()const
	{
		return num_layers;
	}
}
