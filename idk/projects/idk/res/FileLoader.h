#pragma once
#include <file/PathHandle.h>
#include <res/ResourceBundle.h>

namespace idk
{
	class MetaBundle;

	class IFileLoader
	{
	public:
		virtual ResourceBundle LoadFile(PathHandle handle) = 0;
		virtual ResourceBundle LoadFile(PathHandle handle, const MetaBundle& meta) = 0;
	};
}