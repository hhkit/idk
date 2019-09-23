#pragma once
#include <file/PathHandle.h>
#include <res/ResourceBundle.h>

namespace idk
{
	class MetaFile;

	class IFileLoader
	{
	public:
		virtual ResourceBundle LoadFile(PathHandle handle) = 0;
		virtual ResourceBundle LoadFile(PathHandle handle, const MetaFile& meta) = 0;
	};
}