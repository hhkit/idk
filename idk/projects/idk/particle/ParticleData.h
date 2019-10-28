#pragma once

#include <idk.h>

namespace idk
{
    struct ParticleData
    {
        vector<float> lifetime;
        vector<float> age;
        vector<vec3> position;
        vector<rad> rotation;
        vector<float> size;
        vector<vec3> velocity_start;
        vector<vec3> velocity_delta;
        vector<color> color;

        uint16_t num_total = 1000;
        uint16_t num_alive = 0;

        void Allocate(uint16_t max_size);
        void Kill(uint16_t i);
    };
}