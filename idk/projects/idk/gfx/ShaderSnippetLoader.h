#pragma once
#include <idk.h>
#include <res/FileLoader.h>
#include <gfx/ShaderSnippet.h>
#include <res/MetaBundle.h>
#include <core/Core.h>
#include <util/ioutils.h>
namespace idk
{
	struct ShaderSnippetLoader : IFileLoader
	{
		ResourceBundle LoadFile(PathHandle handle, const MetaBundle& bundle) override
		{
			auto meta = bundle.FetchMeta<ShaderSnippet>();
			auto snippet = meta
				? Core::GetResourceManager().LoaderEmplaceResource<ShaderSnippet>(meta->guid)
				: Core::GetResourceManager().LoaderEmplaceResource<ShaderSnippet>();
			
			snippet->snippet = stringify(handle.Open(FS_PERMISSIONS::READ));
			return snippet;
		}
	};
}