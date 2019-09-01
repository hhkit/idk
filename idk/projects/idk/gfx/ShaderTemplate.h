#pragma once
#include <res/Resource.h>

namespace idk
{
	class ShaderTemplate final
		: public Resource<ShaderTemplate>
	{
	public:
		static constexpr string_view uniform_replacer = "//__MATERIAL_UNIFORMS__;";
		static constexpr string_view code_replacer    = "//__MATERIAL_CODE__;";

		ShaderTemplate(string_view template_code);
		string Instantiate(string_view material_uniforms, string_view material_code) const;
	private:
		string template_code;
	};
}