#pragma once
#include <gfx/FramebufferFactory.h>
namespace idk::vkn
{
	class VknFrameBufferFactory : public FrameBufferFactory
	{
	public:
		VknFrameBufferFactory();
		unique_ptr<FrameBuffer> Create()override;	// generate default resource - the fallback resource if a handle fails
		unique_ptr<FrameBuffer> GenerateDefaultResource()override;	// generate default resource - the fallback resource if a handle fails
	private:
		struct Pimpl;
		unique_ptr<Pimpl> _pimpl;
		//out must be assigned a make unique of the implementation version of attachment
		void CreateAttachment(AttachmentType type, const AttachmentInfo& info, ivec2 size, unique_ptr<Attachment>& out) override;
		void PreReset(FrameBuffer& framebuffer) override;//resets the framebuffer (queue resource for destruction)
		
		virtual void Finalize(FrameBuffer& h_fb) override;
	};
}
