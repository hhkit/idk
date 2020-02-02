#include "stdafx.h"
#include "ShaderSnippetLoader.h"
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>
#include <res/Guid.inl>
#include <res/MetaBundle.inl>
namespace idk
{
// // #pragma optimize("",off)
ResourceBundle ShaderSnippetLoader::LoadFile(PathHandle handle, const MetaBundle& bundle)
{
	auto meta = bundle.FetchMeta<ShaderSnippet>();
	auto snippet = meta
		? Core::GetResourceManager().LoaderEmplaceResource<ShaderSnippet>(meta->guid)
		: Core::GetResourceManager().LoaderEmplaceResource<ShaderSnippet>();
	auto& snip = *snippet;
	snip.snippet = stringify(handle.Open(FS_PERMISSIONS::READ));
	return snippet;
}

}