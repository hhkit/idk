#include "stdafx.h"
#include "Material.h"

namespace idk
{
	RscHandle<ShaderProgram> Material::GetShaderProgram() const
	{
		return _program;
	}
	void Material::OnMetaUpdate(const MaterialMeta& newmeta)
	{
		_program.guid = newmeta.compiled_shader_guid;
	}
}
