#pragma once

#include <math/color.h>
#include <util/enum.h>

namespace idk
{
    class ParticleSystem;

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
        bool in_world_space = false;
        bool play_on_awake = true;
    };

    struct EmissionModule
    {
        bool enabled = true;
        float rate_over_time = 10.0f;
    };

    struct ShapeModule
    {
        bool enabled = true;

        // sphere
        float radius = 1.0f;
        float radius_thickness = 1.0f;
        float randomize_direction = 0;

        void Generate(ParticleSystem& data, uint16_t i);
    };

    struct RendererModule
    {
        static constexpr Guid default_material_inst{ 0xbfa6c3e0, 0x9b52, 0x412b, 0xb3e7c85ee93190ca };

        bool enabled = true;
        RscHandle<MaterialInstance> material{ default_material_inst };
    };
}