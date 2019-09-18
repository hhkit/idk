#pragma once
#include <res/ExtensionLoader.h>

namespace idk
{
	class OpenGLCubeMapLoader
		: public ExtensionLoader
	{
	public:
		FileResources Create(PathHandle path_to_resource) override;
		FileResources Create(PathHandle path_to_resource, const MetaFile& path_to_meta) override;

	private:


	};
};