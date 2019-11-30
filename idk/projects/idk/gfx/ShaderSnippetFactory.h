#pragma once
#include <idk.h>
#include <res/EasyFactory.h>
#include <gfx/ShaderSnippet.h>
namespace idk
{
	using ShaderSnippetFactory = EasyFactory<ShaderSnippet>;
}