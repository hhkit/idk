#pragma once
#include <idk.h>
#include <res/Resource.h>
#include <res/ResourceMeta.h>
#include <gfx/TextureRenderMeta.h>

namespace idk {

	struct FontAtlasMeta
	{
		string font_name = {};
		UVMode      uv_mode = UVMode::ClampToBorder;
		ColorFormat internal_format = ColorFormat::R_16;
		InputChannels format = InputChannels::RED;   //Remove, loader determines this
		FilterMode  filter_mode = FilterMode::Linear;
		bool is_srgb{ true };
		int  fontSize = 48;
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
		struct character_info {
			
			//advance
			vec2 advance;

			//Size of glyph	
			//x = width, y = rows
			vec2 glyph_size;

			//Bearing		
			//x = left, y = top
			vec2 bearing;

			vec2 tex_offset; // x offset of glyph in texture coordinates
		} c[128];

		FontAtlas() = default;
		virtual void* ID() const { return 0; };

		//Dtor
		virtual ~FontAtlas() = default;

		// accessors
		float AspectRatio() const;
		ivec2 Size() const;

		// modifiers
		virtual void Size(ivec2 newsize);

		PathHandle reload_path{};

	protected:
		//Size for each texture in the cubemap
		ivec2 _size{};
		void OnMetaUpdate(const FontAtlasMeta&) {};
	};
}