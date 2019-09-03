#pragma once
#include <gfx/RenderTarget.h>
#include <res/ResourceFactory.h>

namespace idk::ogl
{
	class FrameBufferFactory
		: public ResourceFactory<RenderTarget>
	{
		unique_ptr<RenderTarget> GenerateDefaultResource() override;
		unique_ptr<RenderTarget> Create() override;
		unique_ptr<RenderTarget> Create(FileHandle fh) override;
		unique_ptr<RenderTarget> Create(FileHandle filepath, const RenderTarget::Metadata& m);
	};
}