#pragma once
#include <res/Resource.h>

namespace idk
{
	class ShaderGraph;

	class Material
		: public Resource<Material>
	{
	public:
		virtual void Set(string_view fragment_code) = 0;
		virtual void Set(const ShaderGraph&) = 0;
		virtual ~Material() = default;
	};
}