#include "pch.h"
#include "VknFrameBufferfactory.h"
#include <core/Core.h>
#include <app/Application.h>
#include <vkn/VknTexture.h>
#include <vkn/VknFrameBuffer.h>

namespace idk::vkn
{
	unique_ptr<RenderTarget> VknFrameBufferFactory::GenerateDefaultResource()
	{
		auto fb = std::make_unique<VknFrameBuffer>();
		auto m = fb->GetMeta();
		m.size = Core::GetSystem<Application>().GetScreenSize();
		m.textures.emplace_back(Core::GetResourceManager().Create<VknTexture>())->Size(m.size);
		fb->SetMeta(m);
		return fb;
	}
	unique_ptr<RenderTarget> VknFrameBufferFactory::Create()
	{
		auto fb = std::make_unique<VknFrameBuffer>();
		auto m = fb->GetMeta();
		m.size = ivec2{ 512, 512 };
		m.textures.emplace_back(Core::GetResourceManager().Create<VknTexture>())->Size(m.size);
		fb->SetMeta(m);
		return fb;
	}
	unique_ptr<RenderTarget> VknFrameBufferFactory::Create(PathHandle fh)
	{
		return unique_ptr<RenderTarget>();
	}

	unique_ptr<RenderTarget> VknFrameBufferFactory::Create(PathHandle filepath, const RenderTarget::Metadata& m)
	{
		auto fb = std::make_unique<VknFrameBuffer>();

		for (auto& elem : m.textures)
		{
			// ensure textures are created
			Core::GetResourceManager().Free(elem);
			Core::GetResourceManager().LoaderEmplaceResource<VknTexture>(elem.guid);
		}
		fb->SetMeta(m);

		return fb;
	}
}