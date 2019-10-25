#include "stdafx.h"
#include "ShaderTemplateLoader.h"
#include <gfx/ShaderTemplate.h>
#include <res/MetaBundle.h>
#include <util/ioutils.h>

namespace idk
{
	ResourceBundle ShaderTemplateLoader::LoadFile(PathHandle path, const MetaBundle& metadatas)
	{
		auto meta = metadatas.FetchMeta<ShaderTemplate>();
		auto stream = path.Open(FS_PERMISSIONS::READ);
		auto ser = stringify(stream);
		return meta ? Core::GetResourceManager().LoaderEmplaceResource<ShaderTemplate>(metadatas.metadatas[0].guid, ser)
			: Core::GetResourceManager().LoaderEmplaceResource<ShaderTemplate>(ser);
	}
}
