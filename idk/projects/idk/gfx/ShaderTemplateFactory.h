#pragma once
#include <res/EasyFactory.h>
#include <gfx/ShaderTemplate.h>

namespace idk
{
	class ShaderTemplateFactory
		: public EasyFactory<ShaderTemplate>
	{
	public:
		unique_ptr<ShaderTemplate> GenerateDefaultResource() override;
	};
}