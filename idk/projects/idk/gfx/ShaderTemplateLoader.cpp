#include "stdafx.h"
#include "ShaderTemplateLoader.h"
#include <gfx/ShaderTemplate.h>
#include <res/MetaBundle.h>
#include <util/ioutils.h>

namespace idk
{
	ResourceBundle ShaderTemplateLoader::LoadFile(PathHandle path)
	{
		auto stream = path.Open(FS_PERMISSIONS::READ);
		return Core::GetResourceManager().LoaderEmplaceResource<ShaderTemplate>(stringify(stream));
	}
	ResourceBundle ShaderTemplateLoader::LoadFile(PathHandle path, const MetaBundle& metadatas)
	{
		auto stream = path.Open(FS_PERMISSIONS::READ);
		return Core::GetResourceManager().LoaderEmplaceResource<ShaderTemplate>(metadatas.metadatas[0].guid, stringify(stream));
	}
}
