#pragma once
#include <idk.h>
#include <res/ResourceFile.h>

namespace idk
{
	class ExtensionLoader
	{
		virtual ResourceFile Create(string_view filename) = 0;
		virtual ~ExtensionLoader() = default;
	};
}