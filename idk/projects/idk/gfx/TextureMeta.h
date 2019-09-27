#pragma once

#include <idk.h>
#include <util/enum.h>

namespace idk
{
    ENUM(ColorFormat, char,
        RGB_8,
        RGBA_8,
        RGBF_16,
        RGBF_32,
        RGBAF_16,
        RGBAF_32
    )

    ENUM(UVMode, char,
        Repeat,
        MirrorRepeat,
        Clamp
    );

    ENUM(InputChannels, char
        , RED
        , RG
        , RGB
        , RGBA
    );

    struct TextureMeta
    {
        ColorFormat internal_format = ColorFormat::RGBF_32;
        UVMode      uv_mode = UVMode::Repeat;
    };
}