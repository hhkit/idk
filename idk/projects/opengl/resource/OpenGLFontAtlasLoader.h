#pragma once
#include "pch.h"
#include <res/FileLoader.h>

#include <ft2build.h>
#include FT_FREETYPE_H 

namespace idk {
	
	class OpenGLFontAtlasLoader
		: public IFileLoader
	{
	public:
		OpenGLFontAtlasLoader();
		ResourceBundle LoadFile(PathHandle path_to_resource, const MetaBundle& path_to_meta) override;

	private:

		FT_Library ft;
	};
}