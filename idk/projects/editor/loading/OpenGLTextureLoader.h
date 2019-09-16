#pragma once
#include <res/ExtensionLoader.h>

namespace idk
{
	class OpenGLTextureLoader
		: public ExtensionLoader
	{
	public:
		FileResources Create(FileHandle path_to_resource) override;
		FileResources Create(FileHandle path_to_resource, const MetaFile& path_to_meta) override;

	private:


	};
};