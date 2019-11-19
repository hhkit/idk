#include "stdafx.h"
#include "MaterialFactory.h"
#include <util/ioutils.h>
#include <res/MetaBundle.h>
#include <gfx/MaterialInstance.h>
#include <gfx/ShaderProgram.h>
namespace idk
{
	unique_ptr<Material> MaterialFactory::GenerateDefaultResource()
	{
		return Create();
	}

	unique_ptr<Material> MaterialFactory::Create()
	{
		auto retval = std::make_unique<Material>();
		retval->_shader_program = Core::GetResourceManager().Create<ShaderProgram>();
		return retval;
	}

	unique_ptr<Material> MaterialFactory::Create(PathHandle h)
	{
		auto stream = h.Open(FS_PERMISSIONS::READ);
		return std::make_unique<Material>(parse_text<Material>(stringify(stream)));
	}
}