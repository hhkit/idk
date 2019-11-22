#pragma once
#include <idk.h>
#include <res/Resource.h>
#include <res/ResourceMeta.h>
#include <gfx/TextureRenderMeta.h>

namespace idk {

	struct FontAtlasMeta
	{
		UVMode uv_mode = UVMode::ClampToBorder;
		ColorFormat internal_format = ColorFormat::R_8;
		InputChannels format = InputChannels::RED;   //Remove, loader determines this
		FilterMode filter_mode = FilterMode::Linear;
        bool is_srgb = true;
		unsigned font_size = 48;
	};

	class FontAtlas
		: public Resource<FontAtlas>
		, public MetaTag<FontAtlasMeta>
	{
	public:
		static constexpr RscHandle<FontAtlas> defaults[FontDefault::count] =
		{
			{ Guid{0x382A438E, 0xADC8, 0x4283, 0xA8D0E339F7D34159} }, // SourceSansPro
		};

		struct character_info
        {
			vec2 advance;
			vec2 glyph_size; // w, h
			vec2 bearing; // left, top
			vec2 tex_offset; // x offset of glyph in texture coordinates
		} c[128];

        PathHandle reload_path;

		// accessors
		float AspectRatio() const;
		ivec2 Size() const;

		// modifiers
		virtual void Size(ivec2 newsize);

        virtual void* ID() const { return 0; };

	protected:
		ivec2 _size{};
		void OnMetaUpdate(const FontAtlasMeta&) override {};
	};
}