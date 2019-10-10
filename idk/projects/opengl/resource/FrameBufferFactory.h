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
	};

	class FrameBufferLoader
		: public IFileLoader
	{
		ResourceBundle LoadFile(PathHandle fh) override;
		ResourceBundle LoadFile(PathHandle filepath, const MetaBundle& m) override;
	};
}