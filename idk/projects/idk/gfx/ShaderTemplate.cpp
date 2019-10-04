#include "stdafx.h"
#include "ShaderTemplate.h"

namespace idk
{
	namespace detail
	{
		auto& replace(string& configureme, string_view findme, string_view replacewithme)
		{
			const auto find = configureme.find(findme);
			if (find != string::npos)
				configureme.replace(find, findme.length(), replacewithme);
			return configureme;
		}
	}

	ShaderTemplate::ShaderTemplate(string_view code)
		: template_code{ code }
	{
	}

	string ShaderTemplate::Instantiate(string_view material_uniforms, string_view material_code) const
	{
		auto replicate = template_code;
		detail::replace(replicate, uniform_replacer, material_uniforms);
		detail::replace(replicate, code_replacer, material_code);
		return replicate;
	}
}