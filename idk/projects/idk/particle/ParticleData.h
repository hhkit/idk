#pragma once

#include <idk.h>

namespace idk
{
    struct ParticleData
    {
        unique_ptr<float[]> lifetimes;
        unique_ptr<vec3[]> positions;
        unique_ptr<float[]> rotations;
        unique_ptr<float[]> sizes;
        unique_ptr<vec3[]> velocities;
        unique_ptr<color[]> colors;

        uint16_t num_total;
        uint16_t num_alive;

        void Allocate(uint16_t max_size);
    };
}