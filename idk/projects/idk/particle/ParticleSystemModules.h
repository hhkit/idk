#pragma once

#include <math/color.h>
#include <util/enum.h>
#include <particle/MinMax.h>

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
        rad start_rotation = rad{ 0 };
        color start_color{ 1.0f, 1.0f, 1.0f, 1.0f };
        float gravity_modifier = 0;
        bool in_world_space = false;
        bool play_on_awake = true;
        bool destroy_on_finish = false;
    };

    struct EmissionModule
    {
        struct Burst
        {
            float time = 0;
            int count = 30;
            int cycles = 1;
            float interval = 0.01f;
            float probability = 1.0f;
        };

        bool enabled = true;
        float rate_over_time = 10.0f;
        vector<Burst> bursts;
    };

    struct ShapeModule
    {
        bool enabled = true;

        // sphere
        float radius = 1.0f;
        float radius_thickness = 1.0f;
        float randomize_direction = 0;

        void Generate(ParticleSystem& system, uint16_t i);
    };

    struct VelocityOverLifetimeModule
    {
        bool enabled = false;
        MinMax<vec3> linear{ vec3{ 0, 0, 0 }, vec3{ 0, 0, 0 } };
        MinMax<vec3> orbital{ vec3{ 0, 0, 0 }, vec3{ 0, 0, 0 } };
        MinMax<vec3> offset{ vec3{ 0, 0, 0 }, vec3{ 0, 0, 0 } };
        MinMax<float> radial{ 0, 0 };

        void Update(ParticleSystem& system, float dt);
    };

    struct ColorOverLifetimeModule
    {
        bool enabled = false;
        MinMax<color> color{ idk::color{ 1.0f, 1.0f, 1.0f, 1.0f }, idk::color{ 1.0f, 1.0f, 1.0f, 1.0f } };

        void Update(ParticleSystem& system);
    };

    struct SizeOverLifetimeModule
    {
        bool enabled = false;
        MinMax<float> size{ 0, 1.0f, MinMaxMode::Linear };

        void Update(ParticleSystem& system);
    };

    struct RotationOverLifetimeModule
    {
        bool enabled = false;
        MinMax<rad> angular_velocity{ deg{ 45.0f }, deg{ 45.0f } };

        void Update(ParticleSystem& system, float dt);
    };

    struct RendererModule
    {
        static constexpr Guid default_material_inst{ 0xbfa6c3e0, 0x9b52, 0x412b, 0xb3e7c85ee93190ca };

        bool enabled = true;
        RscHandle<MaterialInstance> material{ default_material_inst };
    };
}