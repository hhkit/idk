#include "stdafx.h"
#include "ShaderTemplateFactory.h"
#include <sstream>

const idk::string_view default_template = R"(
#version 460

out vec4 out_color;

void main()
{
	vec4 color = vec4(1.0);

	//__MATERIAL_CODE__;

	out_color = color;
})";

namespace idk
{
	unique_ptr<ShaderTemplate> ShaderTemplateFactory::GenerateDefaultResource()
	{
		return std::make_unique<ShaderTemplate>(default_template);
	}
	unique_ptr<ShaderTemplate> ShaderTemplateFactory::Create(FileHandle f)
	{
		auto stream = f.Open(FS_PERMISSIONS::READ);
		std::stringstream stringify;
		stringify << stream.rdbuf();

		if (stream)
			return std::make_unique<ShaderTemplate>(stringify.str());
		else
			return nullptr;
	}
}