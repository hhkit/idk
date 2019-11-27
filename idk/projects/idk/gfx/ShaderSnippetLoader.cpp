#include "stdafx.h"
#include "ShaderSnippetLoader.h"

namespace idk
{
#pragma optimize("",off)
ResourceBundle ShaderSnippetLoader::LoadFile(PathHandle handle, const MetaBundle& bundle)
{
	auto meta = bundle.FetchMeta<ShaderSnippet>();
	auto snippet = meta
		? Core::GetResourceManager().LoaderEmplaceResource<ShaderSnippet>(meta->guid)
		: Core::GetResourceManager().LoaderEmplaceResource<ShaderSnippet>();

	snippet->snippet = stringify(handle.Open(FS_PERMISSIONS::READ));
	return snippet;
}

}