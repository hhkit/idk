#pragma once
#include <idk.h>
#include <res/Resource.h>
#include <res/ResourceMeta.h>
#include <gfx/TextureRenderMeta.h>
#include <file/PathHandle.h>
#include <res/Guid.inl>

namespace idk {

	struct FontAtlasMeta
	{
		UVMode uv_mode = UVMode::ClampToBorder;
		ColorFormat internal_format = ColorFormat::R_8;
		FilterMode filter_mode = FilterMode::Linear;
        bool is_srgb = true;
		unsigned font_size = 48;
	};

	class FontAtlas
		: public MetaResource<FontAtlas, FontAtlasMeta>
	{
	public:
		static constexpr RscHandle<FontAtlas> defaults[FontDefault::count] =
		{
			{ Guid{0x382A438E, 0xADC8, 0x4283, 0xA8D0E339F7D34159} }, // SourceSansPro
		};

		//Characters information
		vector<CharacterInfo> char_map;
        // max y above baseline
        float ascender;
        // max y below baseline
        float descender;

        PathHandle reload_path;

		FontAtlas() = default;
		virtual ~FontAtlas() = default;

		// accessors
		float AspectRatio() const;
		uvec2 Size() const;

		// modifiers
		virtual void Size(uvec2 newsize);

        virtual void* ID() const { return 0; };

	protected:
		uvec2 _size{};
	};
}