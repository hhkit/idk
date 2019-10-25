#include "stdafx.h"
#include "ParticleSystem.h"

namespace idk
{

    void ParticleSystem::Step(float dt)
    {
        if (state == Paused)
            return;
        if (state == Stopped)
        {
            data.Allocate(main.max_particles);
            state = Playing;
        }

        time += dt;
        if (time < main.start_delay)
            return;

        emitter_clock += dt * emission.rate_over_time;
        int emit_count = static_cast<int>(emitter_clock);
        emitter_clock -= emit_count;
        while (emit_count-- > 0)
            Emit();

        for (uint16_t i = 0; i < data.num_alive; ++i)
        {
            data.lifetimes[i] -= dt;
            if (data.lifetimes[i] <= 0)
            {
                --data.num_alive;
                data.lifetimes[i] = data.lifetimes[data.num_alive];
                data.positions[i] = data.positions[data.num_alive];
                data.rotations[i] = data.rotations[data.num_alive];
                data.sizes[i] = data.sizes[data.num_alive];
                data.velocities[i] = data.velocities[data.num_alive];
                data.colors[i] = data.colors[data.num_alive];
                continue;
            }
        }
        for (uint16_t i = 0; i < data.num_alive; ++i)
            data.positions[i] += data.velocities[i];
    }

    void ParticleSystem::Emit()
    {
        if (data.num_alive == data.num_total)
            return;

        auto i = data.num_alive++;
        data.lifetimes[i] = main.start_lifetime;
        data.positions[i] = vec3(0, 0, 0);
        data.rotations[i] = main.start_rotation;
        data.sizes[i] = main.start_size;
        data.velocities[i] = vec3(0, main.start_speed, 0);
        data.colors[i] = main.start_color;
    }

}
