#pragma once
#include <res/ResourceFactory.h>
#include <gfx/ShaderTemplate.h>

namespace idk
{
	class ShaderTemplateFactory
		: public ResourceFactory<ShaderTemplate>
	{
	public:
		unique_ptr<ShaderTemplate> GenerateDefaultResource() override;
		unique_ptr<ShaderTemplate> Create(PathHandle) override;
	};
}