#pragma once
#include <idk.h>
#include <res/FileLoader.h>

namespace idk
{
	template<typename Res>
	class SaveableResourceLoader
		: public IFileLoader
	{
	public:
		using Resource = Res;

		ResourceBundle LoadFile(PathHandle p, const MetaBundle& bundle);
	};
}

#include "SaveableResourceLoader.inl"