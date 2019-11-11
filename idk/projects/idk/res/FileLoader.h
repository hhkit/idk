#pragma once
#include <file/PathHandle.h>
#include <res/ResourceBundle.h>

namespace idk
{
	struct MetaBundle;

	class IFileLoader
	{
	public:
		virtual ResourceBundle LoadFile(PathHandle handle, const MetaBundle& meta) = 0;
		virtual ~IFileLoader() = default;
	};
}