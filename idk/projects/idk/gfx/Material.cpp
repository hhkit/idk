#include "stdafx.h"
#include "Material.h"

namespace idk
{
	RscHandle<ShaderProgram> Material::GetShaderProgram() const
	{
		return _program;
	}
}
