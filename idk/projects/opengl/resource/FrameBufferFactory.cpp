#include "pch.h"
#include "FrameBufferfactory.h"
#include <core/Core.h>
#include <app/Application.h>
#include <opengl/resource/OpenGLTexture.h>
#include <opengl/resource/FrameBuffer.h>
#include <opengl/resource/OpenGLCubemap.h>
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>
#include <res/Guid.inl>

namespace idk::ogl
{
	unique_ptr<RenderTarget> OpenGLRenderTargetFactory::GenerateDefaultResource()
	{
		auto fb = std::make_unique<OpenGLRenderTarget>();
		auto& m = *fb;
		m.Size(uivec2{ Core::GetSystem<Application>().GetScreenSize() });
		m.SetColorBuffer(RscHandle<Texture>{Core::GetResourceManager().LoaderEmplaceResource<OpenGLTexture>(TextureInternalFormat::RGB_16_F, m.size)});
		m.SetDepthBuffer(RscHandle<Texture>{Core::GetResourceManager().LoaderEmplaceResource<OpenGLTexture>(TextureInternalFormat::DEPTH_16, m.size)});

		fb->Name("Game View");
        m.render_debug = false;

		return fb;
	}

	unique_ptr<RenderTarget> OpenGLRenderTargetFactory::Create()
	{
		auto fb = std::make_unique<OpenGLRenderTarget>();
		auto &m = *fb;
		m.Size(uivec2{ 512,512 });
		m.SetColorBuffer(RscHandle<Texture>{Core::GetResourceManager().LoaderEmplaceResource<OpenGLTexture>(TextureInternalFormat::RGB_16_F, uivec2{ 512,512 })});
		m.SetDepthBuffer(RscHandle<Texture>{Core::GetResourceManager().LoaderEmplaceResource<OpenGLTexture>(TextureInternalFormat::DEPTH_16, uivec2{ 512,512 })});
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

	void OpenGLFrameBufferFactory::CreateAttachment(AttachmentType type, const AttachmentInfo& info, uivec2 size, unique_ptr<Attachment>& out)
	{
		type;
		out = std::make_unique<OpenGLAttachment>();
		out->load_op  = info.load_op;
		out->store_op = info.store_op;

		out->buffer = Core::GetResourceManager().LoaderEmplaceResource<OpenGLTexture>(info.internal_format, size);
	}
	void OpenGLFrameBufferFactory::PreReset(FrameBuffer&) {}
	void OpenGLFrameBufferFactory::Finalize(FrameBuffer&, [[maybe_unused]]SpecializedInfo* info) {}
}