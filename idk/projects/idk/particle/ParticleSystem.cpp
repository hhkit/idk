#include "stdafx.h"
#include "ParticleSystem.h"
#include <core/GameObject.h>
#include <gfx/RenderObject.h>

namespace idk
{
    void ParticleSystem::Play()
    {
        if (state == Stopped || state == Awake)
            data.Allocate(main.max_particles);
        state = Playing;
    }

    void ParticleSystem::Pause()
    {
        state = Paused;
    }

    void ParticleSystem::Stop()
    {
        state = Stopped;
        time = 0;
        emitter_clock = 0;
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
                state = Stopped;
        }

        time += dt;
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
            emitter_clock += dt * emission.rate_over_time;
            int emit_count = static_cast<int>(emitter_clock);
            emitter_clock -= emit_count;
            while (emit_count-- > 0)
                Emit();
        }

        for (uint16_t i = 0; i < data.num_alive; ++i)
        {
            data.age[i] += dt;
            if (data.age[i] >= data.lifetime[i])
            {
                data.Kill(i);
                continue;
            }
        }

        velocity_over_lifetime.Update(*this, dt);
        color_over_lifetime.Update(*this);
        size_over_lifetime.Update(*this);
        rotation_over_lifetime.Update(*this, dt);

        for (uint16_t i = 0; i < data.num_alive; ++i)
            data.position[i] += (data.velocity_start[i] + data.velocity_delta[i] +
                                 vec3(0, -9.81f, 0) * main.gravity_modifier * data.age[i]) * dt;
    }

    void ParticleSystem::Emit()
    {
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

        if (main.in_world_space)
            data.position[i] = transform.position + mat3(transform.rotation) * data.position[i];
    }

}
