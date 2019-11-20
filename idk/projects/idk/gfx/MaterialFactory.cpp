#include "stdafx.h"
#include "MaterialFactory.h"
#include <util/ioutils.h>
#include <res/MetaBundle.h>
#include <gfx/Material.h>
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
		retval->_default_instance = Guid::Make();
		Core::GetResourceManager().Create<MaterialInstance>(retval->_default_instance.guid);
		retval->_shader_program = Core::GetResourceManager().Create<ShaderProgram>();
		return retval;
	}

	unique_ptr<Material> MaterialFactory::Create(PathHandle h)
	{
		auto stream = h.Open(FS_PERMISSIONS::READ);
		auto retval = std::make_unique<Material>();
		parse_text(stringify(stream), *retval);
		Core::GetResourceManager().Create<MaterialInstance>(retval->_default_instance.guid);
		Core::GetResourceManager().Create<ShaderProgram>(retval->_shader_program.guid);
		return retval;
	}
}