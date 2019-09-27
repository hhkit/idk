#include "stdafx.h"
#include "ShaderGraphFactory.h"
#include <util/ioutils.h>
#include <serialize/serialize.h>
#include <res/MetaBundle.h>

namespace idk::shadergraph
{

	ResourceBundle Loader::LoadFile(PathHandle file_handle)
	{
		auto fs = file_handle.Open(FS_PERMISSIONS::READ);
		auto str = stringify(fs);
		auto handle = Core::GetResourceManager().LoaderEmplaceResource<Graph>();
		parse_text(str, *handle);
		return handle;
	}

	ResourceBundle Loader::LoadFile(PathHandle file_handle, const MetaBundle& meta)
	{
		auto fs = file_handle.Open(FS_PERMISSIONS::READ);
		auto str = stringify(fs);
		auto handle = Core::GetResourceManager().LoaderEmplaceResource<Graph>(meta.metadatas[0].guid);
		parse_text(str, *handle);
		return handle;
	}

}
