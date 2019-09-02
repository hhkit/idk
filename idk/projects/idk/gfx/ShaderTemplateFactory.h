#pragma once
#include <res/ResourceFactory.h>
#include <gfx/ShaderTemplate.h>

namespace idk
{
	class ShaderTemplateFactory
		: public ResourceFactory<ShaderTemplate>
	{
	public:
		unique_ptr<ShaderTemplate> Create() override;
		unique_ptr<ShaderTemplate> Create(FileHandle) override;
	};
}