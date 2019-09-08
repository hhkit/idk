#include "pch.h"
#include "FrameBufferfactory.h"
#include <core/Core.h>
#include <app/Application.h>
#include <idk_opengl/resource/OpenGLTexture.h>
#include <idk_opengl/resource/FrameBuffer.h>

namespace idk::ogl
{
	unique_ptr<RenderTarget> FrameBufferFactory::GenerateDefaultResource()
	{
		auto fb = std::make_unique<FrameBuffer>();
		auto m = fb->GetMeta();
		m.size = Core::GetSystem<Application>().GetScreenSize();
		m.textures.emplace_back(Core::GetResourceManager().Create<OpenGLTexture>())->Size(m.size);
		fb->SetMeta(m);
		return fb;
	}
	unique_ptr<RenderTarget> FrameBufferFactory::Create()
	{
		auto fb = std::make_unique<FrameBuffer>();
		auto m = fb->GetMeta();
		m.size = ivec2{ 512, 512 };
		m.textures.emplace_back(Core::GetResourceManager().Create<OpenGLTexture>())->Size(m.size);
		fb->SetMeta(m);
		return fb;
	}
	unique_ptr<RenderTarget> FrameBufferFactory::Create(FileHandle fh)
	{
		return unique_ptr<RenderTarget>();
	}

	unique_ptr<RenderTarget> FrameBufferFactory::Create(FileHandle filepath, const RenderTarget::Metadata& m)
	{
		auto fb = std::make_unique<FrameBuffer>();

		for (auto& elem : m.textures)
		{
			// ensure textures are created
			Core::GetResourceManager().Free(elem);
			Core::GetResourceManager().Create<OpenGLTexture>(elem.guid);
		}
		fb->SetMeta(m);

		return fb;
	}
}