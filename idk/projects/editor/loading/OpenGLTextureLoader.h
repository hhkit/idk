#pragma once
#include <res/FileLoader.h>

namespace idk
{
	class OpenGLTextureLoader
		: public IFileLoader
	{
	public:
		ResourceBundle LoadFile(PathHandle path_to_resource) override;
		ResourceBundle LoadFile(PathHandle path_to_resource, const MetaBundle& path_to_meta) override;

	private:


	};
};