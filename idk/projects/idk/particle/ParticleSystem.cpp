#include "stdafx.h"
#include "ParticleSystem.h"
#include <core/GameObject.inl>
#include <gfx/RenderObject.h>
#include <math/arith.inl>

namespace idk
{
    void ParticleSystem::Play()
    {
        if (state == Stopped || state == Awake)
            data.Allocate(main.max_particles);
        state = Playing;

        if (main.prewarm && time == 0)
        {
            Step(main.duration);
        }
    }

    void ParticleSystem::Pause()
    {
        state = Paused;
    }

    void ParticleSystem::Stop()
    {
        state = Stopped;
        time = 0;
        emission.clock = 0;
        for (auto& burst : emission.bursts)
        {
            burst.cycle_counter = 0;
            burst.clock = 0;
        }
        cycle_time = 0;
        data.num_alive = 0;
    }

    void ParticleSystem::Step(float dt)
    {
        if (state == Stopped || state == Paused)
            return;
        if (state == Awake)
        {
            if (main.play_on_awake)
                Play();
            else
            {
                state = Stopped;
                return;
            }
        }

        const auto ForwardParticleAge = [&](float dur)
        {
            while (dur > 0)
            {
                constexpr auto max_timeslice = 1 / 30.0f;
                const float timeslice = dur > max_timeslice ? max_timeslice : dur;
                for (uint16_t i = 0; i < data.num_alive; ++i)
                {
                    data.age[i] += timeslice;
                    if (data.age[i] >= data.lifetime[i])
                    {
                        data.Kill(i);
                        continue;
                    }
                }
                velocity_over_lifetime.Update(*this, timeslice);
                rotation_over_lifetime.Update(*this, timeslice);
                for (uint16_t i = 0; i < data.num_alive; ++i)
                    data.position[i] += (data.velocity_start[i] + data.velocity_delta[i] +
                        vec3(0, -9.81f, 0) * main.gravity_modifier * data.age[i]) * timeslice;
                dur -= timeslice;
            }
        };

        time += dt;
        cycle_time += dt;
        if (time < main.start_delay)
            return;
        if (!main.looping && time >= main.duration)
        {
            if (data.num_alive == 0)
            {
                Stop();
                if (main.destroy_on_finish)
                    GameState::GetGameState().DestroyObject(GetGameObject());
            }
        }
        else if (emission.enabled)
        {
            while (cycle_time >= main.duration)
                cycle_time -= main.duration;
            while (dt >= main.duration)
            {
                dt -= main.duration;
                ForwardParticleAge(main.duration);
                emission.Update(*this, main.duration);
            }

            emission.Update(*this, dt);
        }

        ForwardParticleAge(dt);
        color_over_lifetime.Update(*this);
        size_over_lifetime.Update(*this);
    }

    void ParticleSystem::Emit(float age)
    {
        if (age > main.start_lifetime)
            return;
        if (data.num_alive == main.max_particles)
            return;

        if (data.position.size() < main.max_particles)
            data.Allocate(main.max_particles);

        auto i = data.num_alive++;
        data.lifetime[i] = main.start_lifetime;
        data.age[i] = 0;
        data.position[i] = vec3(0, 0, 0);
        data.rotation[i] = main.start_rotation;
        data.size[i] = main.start_size;
        data.velocity_start[i] = vec3(0, main.start_speed, 0);
        data.velocity_delta[i] = vec3(0, 0, 0);
        data.color[i] = main.start_color;

        shape.Generate(*this, i);

        while (age > 0)
        {
            constexpr auto timeslice = 1 / 30.0f;
            float dt = age > timeslice ? timeslice : age;
            data.age[i] += dt;
            velocity_over_lifetime.Update(*this, dt, i);
            rotation_over_lifetime.Update(*this, dt, i);
            data.position[i] += (data.velocity_start[i] + data.velocity_delta[i] +
                vec3(0, -9.81f, 0) * main.gravity_modifier * data.age[i]) * dt;
            age -= dt;
        }

        if (main.in_world_space)
            data.position[i] = transform.position + mat3(transform.rotation) * data.position[i];
    }

}
