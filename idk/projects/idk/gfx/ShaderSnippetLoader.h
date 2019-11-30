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
		ResourceBundle LoadFile(PathHandle handle, const MetaBundle& bundle) override;
	};
}