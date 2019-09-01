#include "stdafx.h"
#include "ShaderTemplate.h"

namespace idk
{
	ShaderTemplate::ShaderTemplate(string_view code)
		: template_code{ code }
	{
	}

	string ShaderTemplate::Instantiate(string_view material_uniforms, string_view material_code) const
	{
		auto replicate = template_code;
		replicate.replace(replicate.find(uniform_replacer), uniform_replacer.length(), material_uniforms);
		replicate.replace(replicate.find(code_replacer), uniform_replacer.length(), material_code);
		return replicate;
	}
}