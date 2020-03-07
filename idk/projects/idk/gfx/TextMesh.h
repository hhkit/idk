#pragma once
#include <idk.h>
#include <core/Component.h>
#include <gfx/FontAtlas.h>
#include <gfx/TextAnchor.h>
#include <gfx/TextAlignment.h>

namespace idk {
	class TextMesh
		: public Component<TextMesh>
	{
	public:
		real letter_spacing = 0;
		real line_height = 1.f; // multiplier
		color color = {1.f,1.f,1.f,1.f};
		unsigned font_size = 48;
		RscHandle<FontAtlas> font = FontAtlas::defaults[FontDefault::SourceSansPro];
		TextAnchor anchor = TextAnchor::MiddleCenter;
		TextAlignment alignment = TextAlignment::Center;

		string text;

    private:
		virtual void* ID() const { return 0; };
	};
}