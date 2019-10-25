#pragma once

#include <math/color.h>

namespace idk
{
    struct MainModule
    {
        float duration = 5.0f;
        bool looping = true;
        bool prewarm = false;
        uint16_t max_particles = 1000;
        float start_delay = 0;
        float start_lifetime = 5.0f;
        float start_speed = 5.0f;
        float start_size = 1.0f;
        float start_rotation = 0;
        color start_color{ 1.0f, 1.0f, 1.0f, 1.0f };
        float gravity_modifier = 0;
    };

    struct EmissionModule
    {
        bool enabled = true;
        float rate_over_time = 10.0f;
    };
}