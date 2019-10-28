#pragma once

#include <util/easing.h>

namespace idk
{

    ENUM(MinMaxMode, char, Constant, Linear, SineIn, SineOut);

    template<typename T>
    struct MinMax
    {
        T min{};
        T max{};
        MinMaxMode mode = MinMaxMode::Constant;

        T Evaluate(float t)
        {
            switch (mode)
            {
            case MinMaxMode::Constant:        return min;
            case MinMaxMode::Linear:          return lerp(min, max, easing::linear(t));
            case MinMaxMode::SineIn:          return lerp(min, max, easing::sine_in(t));
            case MinMaxMode::SineOut:         return lerp(min, max, easing::sine_out(t));
            default: return min;
            }
        }
    };

}