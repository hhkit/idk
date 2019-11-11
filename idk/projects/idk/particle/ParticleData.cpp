#include "stdafx.h"
#include "ParticleData.h"
#include <math/arith.h>

namespace idk
{

    void ParticleData::Allocate(uint16_t max_size)
    {
        lifetime.resize(max_size);
        age.resize(max_size);
        position.resize(max_size);
        rotation.resize(max_size);
        size.resize(max_size);
        velocity_start.resize(max_size);
        velocity_delta.resize(max_size);
        color.resize(max_size);
    }

    void ParticleData::Kill(uint16_t i)
    {
        --num_alive;
        lifetime[i] = lifetime[num_alive];
        age[i] = age[num_alive];
        position[i] = position[num_alive];
        rotation[i] = rotation[num_alive];
        size[i] = size[num_alive];
        velocity_start[i] = velocity_start[num_alive];
        velocity_delta[i] = velocity_delta[num_alive];
        color[i] = color[num_alive];
    }

}