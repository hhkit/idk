#include "pch.h"
#include "FrameBufferfactory.h"
#include <core/Core.h>
#include <app/Application.h>
#include <opengl/resource/OpenGLTexture.h>
#include <opengl/resource/FrameBuffer.h>
#include <opengl/resource/OpenGLCubemap.h>

namespace idk::ogl
{
	unique_ptr<RenderTarget> OpenGLRenderTargetFactory::GenerateDefaultResource()
	{
		auto fb = std::make_unique<OpenGLRenderTarget>();
		auto& m = *fb;
		m.Size(Core::GetSystem<Application>().GetScreenSize());
		//m.textures.emplace_back(Core::GetResourceManager().Create<OpenGLTexture>())->Size(m.size);
		auto tex = Core::GetResourceManager().Create<OpenGLTexture>();
		tex->Size(m.size);
		m.SetColorBuffer(RscHandle<Texture>{tex});
		tex = Core::GetResourceManager().Create<OpenGLTexture>();
		tex->Size(m.size);
		m.SetDepthBuffer(RscHandle<Texture>{tex});
		fb->Name("None");
		return fb;
	}

	unique_ptr<RenderTarget> OpenGLRenderTargetFactory::Create()
	{
		auto fb = std::make_unique<OpenGLRenderTarget>();
		auto &m = *fb;
		m.Size(ivec2{ 512, 512 });
		auto tex = Core::GetResourceManager().Create<OpenGLTexture>();
		tex->Size(m.size);
		m.SetColorBuffer(RscHandle<Texture>{tex});
		tex = Core::GetResourceManager().Create<OpenGLTexture>();
		tex->Size(m.size);
		m.SetDepthBuffer(RscHandle<Texture>{tex});
		return fb;
	}

	unique_ptr<FrameBuffer> OpenGLFrameBufferFactory::GenerateDefaultResource()
	{
		return unique_ptr<FrameBuffer>{};//it's gonna be null cause you shouldn't be using this.
	}
	unique_ptr<FrameBuffer> OpenGLFrameBufferFactory::Create()
	{
		return std::make_unique<OpenGLFrameBuffer>();
	}
	void OpenGLFrameBufferFactory::CreateAttachment(AttachmentType type, const AttachmentInfo& info, ivec2 size, unique_ptr<Attachment>& out)
	{
		type;
		out = std::make_unique<OpenGLAttachment>();
		out->load_op  = info.load_op;
		out->store_op = info.store_op;

		/*if (type == AttachmentType::eDepth)
		{
			RscHandle<OpenGLTexture> tex = Core::GetResourceManager().Create<OpenGLTexture>();
			tex->Buffer(nullptr, size, InputChannels::RGB, info.internal_format);
			out->buffer = tex;
		}
		else if(type == AttachmentType::eDepth3D)
		{
			RscHandle<OpenGLCubemap> tex = Core::GetResourceManager().Create<OpenGLCubemap>();
			for (int i = 0; i < 6; ++i)
			{
				tex->Buffer(i,nullptr, size, InputChannels::RGB, info.internal_format);
			}
			out->buffer = tex->Tex();
		}*/

		RscHandle<OpenGLTexture> tex = Core::GetResourceManager().Create<OpenGLTexture>();
		tex->Buffer(nullptr, size, InputChannels::RGB, info.internal_format);
		out->buffer = tex;
	}
	void OpenGLFrameBufferFactory::PreReset(FrameBuffer&) {}
	void OpenGLFrameBufferFactory::Finalize(FrameBuffer& ) {}
}