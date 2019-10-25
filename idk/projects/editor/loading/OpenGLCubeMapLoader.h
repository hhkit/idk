#pragma once
#include <res/FileLoader.h>

namespace idk
{
	class OpenGLCubeMapLoader
		: public IFileLoader
	{
	public:
		ResourceBundle LoadFile(PathHandle path_to_resource, const MetaBundle& path_to_meta) override;
	};
};