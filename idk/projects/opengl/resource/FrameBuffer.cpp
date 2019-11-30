#include "pch.h"
#include "FrameBuffer.h"
#include <core/Core.h>
#include <opengl/resource/OpenGLTexture.h>
#include <opengl/resource/FrameBufferFactory.h>

namespace idk::ogl
{
	OpenGLRenderTarget::OpenGLRenderTarget()
	{
	}

	void OpenGLRenderTarget::OnFinalize()
	{
		for (auto& elem : Textures())
			Core::GetResourceManager().Free(elem);

		for (auto& elem : Textures())
		{
			auto tex = elem = (elem == RscHandle<Texture>{})? Core::GetResourceManager().Create<OpenGLTexture>():Core::GetResourceManager().LoaderEmplaceResource<OpenGLTexture>(elem.guid);
			tex->Size(size);
		}
		auto tex = Textures()[kDepthIndex];
		auto tmeta = tex->GetMeta();
		tmeta.internal_format = ColorFormat::DEPTH_COMPONENT;
		tex->SetMeta(tmeta);
		depthbuffer = s_cast<GLuint>(r_cast<intptr_t>(tex->ID()));

		{
			FrameBufferBuilder builder;
			builder.Begin(size);
			builder.AddAttachment(
				AttachmentInfo
				{
					LoadOp::eClear,
					StoreOp::eStore,
					idk::ColorFormat::RGBAF_32,
					FilterMode::Linear
				}
			);
			builder.AddAttachment(
				AttachmentInfo
				{
					LoadOp::eClear,
					StoreOp::eStore,
					idk::ColorFormat::RGBAF_32,
					FilterMode::Linear
				}
			);
			builder.AddAttachment(
				AttachmentInfo
				{
					LoadOp::eClear,
					StoreOp::eStore,
					idk::ColorFormat::RGBF_32,
					FilterMode::Linear
				}
			);
			builder.AddAttachment(
				AttachmentInfo
				{
					LoadOp::eClear,
					StoreOp::eStore,
					idk::ColorFormat::RGBF_32,
					FilterMode::Linear
				}
			);
			builder.AddAttachment(
				AttachmentInfo
				{
					LoadOp::eClear,
					StoreOp::eStore,
					idk::ColorFormat::RGBF_32,
					FilterMode::Linear
				}
			);
			builder.SetDepthAttachment(
				AttachmentInfo
				{
					LoadOp::eClear,
					StoreOp::eStore,
					idk::ColorFormat::DEPTH_COMPONENT,
					FilterMode::_enum::Linear
				}
			);
			pbr_metallic_gbuffer = Core::GetResourceManager().GetFactory<FrameBufferFactory>().Create(builder.End());
		}
	}

	GLuint OpenGLRenderTarget::DepthBuffer() const
	{
		return depthbuffer;
	}
	RscHandle<FrameBuffer> OpenGLRenderTarget::DeferredBufferPBRMetallic() const
	{
		return pbr_metallic_gbuffer;
	}
}