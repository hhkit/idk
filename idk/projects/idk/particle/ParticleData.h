#pragma once

#include <idk.h>

namespace idk
{
    struct ParticleData
    {
        vector<float> lifetimes;
        vector<vec3> positions;
        vector<float> rotations;
        vector<float> sizes;
        vector<vec3> velocities;
        vector<color> colors;

        uint16_t num_total = 1000;
        uint16_t num_alive = 0;

        void Allocate(uint16_t max_size);
    };
}