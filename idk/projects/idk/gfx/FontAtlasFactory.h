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
		unique_ptr<FontAtlas> GenerateDefaultResource() override;
		unique_ptr<FontAtlas> Create() override;
		unique_ptr<FontAtlas> Create(PathHandle h) override;
		void Init() override final; // force inheritors to create default font atlas
		virtual void GenerateDefaultFontAtlas() = 0;
	};
}