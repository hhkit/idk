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
		//m.textures.emplace_back(Core::GetResourceManager().Create<OpenGLTexture>())->Size(m.size);
		auto tex = Core::GetResourceManager().Create<OpenGLTexture>();
		tex->Size(m.size);
		m.textures[RenderTarget::kColorIndex] = tex;
		tex = Core::GetResourceManager().Create<OpenGLTexture>();
		tex->Size(m.size);
		m.textures[RenderTarget::kDepthIndex] = tex;
		fb->SetMeta(m);
		return fb;
	}

	unique_ptr<RenderTarget> FrameBufferFactory::Create()
	{
		auto fb = std::make_unique<FrameBuffer>();
		auto m = fb->GetMeta();
		m.size = ivec2{ 512, 512 };
		auto tex = Core::GetResourceManager().Create<OpenGLTexture>();
		tex->Size(m.size);
		m.textures[RenderTarget::kColorIndex] = tex;
		tex = Core::GetResourceManager().Create<OpenGLTexture>();
		tex->Size(m.size);
		m.textures[RenderTarget::kDepthIndex] = tex;
		fb->SetMeta(m);
		return fb;
	}

	ResourceBundle FrameBufferLoader::LoadFile(PathHandle fh)
	{
		fh;
		assert(false);
		return ResourceBundle();
	}
	ResourceBundle FrameBufferLoader::LoadFile(PathHandle filepath, const MetaBundle& m)
	{
		filepath;
		m;
		assert(false);
		/*
		auto fb = std::make_unique<FrameBuffer>();

		for (auto& elem : m.textures)
		{
			// ensure textures are created
			Core::GetResourceManager().Free(elem);
			Core::GetResourceManager().Emplace<OpenGLTexture>(elem.guid);
		}
		fb->SetMeta(m);

		return fb;
		*/
		return ResourceBundle{};
	}
}