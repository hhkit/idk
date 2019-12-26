#include "stdafx.h"
#include "ShaderTemplateLoader.h"
#include <gfx/ShaderTemplate.h>
#include <res/MetaBundle.inl>
#include <util/ioutils.h>
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>
#include <res/Guid.inl>
namespace idk
{
	ResourceBundle ShaderTemplateLoader::LoadFile(PathHandle path, const MetaBundle& metadatas)
	{
		auto meta = metadatas.FetchMeta<ShaderTemplate>();
		auto stream = path.Open(FS_PERMISSIONS::READ);
		auto ser = stringify(stream);
		auto result = meta ? Core::GetResourceManager().LoaderEmplaceResource<ShaderTemplate>(metadatas.metadatas[0].guid, ser)
			: Core::GetResourceManager().LoaderEmplaceResource<ShaderTemplate>(ser);
		result->Name(path.GetFileName());
		return result;
	}
}
