#pragma once
#include <res/Resource.h>

namespace idk
{
	class ShaderProgram
		: public Resource<ShaderProgram>
	{
	public:
		virtual ~ShaderProgram() = default;
	};
}