#pragma once
#include <idk.h>
#include <gfx/FontAtlas.h>
#include <res/ResourceFactory.h>

namespace idk
{
	class FontAtlasFactory
		: public ResourceFactory<FontAtlas>
	{
	public:
		//unique_ptr<FontAtlas> GenerateDefaultResource() override;
		//unique_ptr<FontAtlas> Create() override;
		void Init() override final; // force inheritors to create default font atlas
		virtual void GenerateDefaultFontAtlas() = 0;
	};

	class FontAtlasLoader
		: public IFileLoader
	{
	public:
		ResourceBundle LoadFile(PathHandle p, const MetaBundle& m) override;
	};
}