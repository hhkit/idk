#pragma once
#include <idk.h>
#include <core/Component.h>
#include <ui/UISystem.h>
#include <ui/TextAnchor.h>
#include <gfx/FontAtlas.h>

namespace idk
{
    class Text
        : public Component<Text>
    {
    public:
        string text;
        RscHandle<FontAtlas> font = FontAtlas::defaults[FontDefault::SourceSansPro];
        RscHandle<MaterialInstance> material{ UISystem::default_material_inst };

        unsigned font_size = 16;
        real letter_spacing = 1.f;
        real line_spacing = 10.f;
        color color{ 1.0f, 1.0f, 1.0f, 1.0f };
        TextAnchor alignment = TextAnchor::MiddleCenter;
        bool wrap = false;
    };
}