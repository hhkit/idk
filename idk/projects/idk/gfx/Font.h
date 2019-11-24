#pragma once
#include <idk.h>
#include <core/Component.h>
#include <util/enum.h>
#include <gfx/FontAtlas.h>
#include <gfx/FontData.h>

namespace idk {
	class Font
		: public Component<Font>
	{
	public:
		real letter_spacing = 0;
		real line_height = 1.f; // multiplier
		color color = {1.f,1.f,1.f,1.f};
		unsigned font_size = 48;
		RscHandle<FontAtlas> font = FontAtlas::defaults[FontDefault::SourceSansPro];

		string text;

    private:
		virtual void* ID() const { return 0; };
	};
}