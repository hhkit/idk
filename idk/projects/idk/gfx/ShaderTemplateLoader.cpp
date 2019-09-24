#include "stdafx.h"
#include "ShaderTemplateLoader.h"
#include <gfx/ShaderTemplate.h>
#include <util/ioutils.h>

namespace idk
{
	ResourceBundle ShaderTemplateLoader::LoadFile(PathHandle path)
	{
		auto stream = path.Open(FS_PERMISSIONS::READ);
		return Core::GetResourceManager().LoaderEmplaceResource<ShaderTemplate>(stringify(stream));
	}
}
