#pragma once
#include <idk.h>
#include <core/Component.h>
#include <ui/UISystem.h>
#include <gfx/TextAnchor.h>
#include <gfx/FontAtlas.h>

namespace idk
{
    class Text
        : public Component<Text>
    {
    public:
        string text;
        RscHandle<FontAtlas> font = FontAtlas::defaults[FontDefault::SourceSansPro];

        unsigned font_size = 48;
        real letter_spacing = 0;
        real line_height = 1.0f;
        color color{ 1.0f, 1.0f, 1.0f, 1.0f };
        TextAnchor alignment = TextAnchor::MiddleCenter;
        bool wrap = false;
        bool best_fit = false;
    };
}