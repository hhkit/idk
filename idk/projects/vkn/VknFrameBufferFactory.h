#pragma once
#include <gfx/RenderTarget.h>
#include <res/ResourceFactory.h>

namespace idk::vkn
{
	class VknFrameBufferFactory
		: public ResourceFactory<RenderTarget>
	{
		unique_ptr<RenderTarget> GenerateDefaultResource() override;
		unique_ptr<RenderTarget> Create() override;
		unique_ptr<RenderTarget> Create(PathHandle fh) override;
		unique_ptr<RenderTarget> Create(PathHandle filepath, const RenderTarget::Metadata& m);
	};
}