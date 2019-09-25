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
	};

	class VknFrameBufferLoader
		: public IFileLoader
	{
	public:
		ResourceBundle LoadFile(PathHandle filepath, const MetaBundle& m);
	};
}