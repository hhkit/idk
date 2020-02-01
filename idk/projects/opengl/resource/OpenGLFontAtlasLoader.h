#pragma once
#include <res/FileLoader.h>

#include <opengl/resource/OpenGLFontAtlas.h>


namespace idk {
	
	namespace ogl {
		ResourceBundle GenerateFontAtlas(PathHandle path_to_resource, RscHandle<OpenGLFontAtlas> font_handle);
		unique_ptr<FontAtlas> GenerateFontAtlas(PathHandle path_to_resource);
	}

	class OpenGLFontAtlasLoader
		: public IFileLoader
	{
	public:
		ResourceBundle LoadFile(PathHandle path_to_resource, const MetaBundle& path_to_meta) override;

	private:
	};
}