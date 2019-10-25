#include "stdafx.h"
#include "ParticleData.h"

namespace idk
{

    void ParticleData::Allocate(uint16_t max_size)
    {
        lifetimes.resize(max_size);
        positions.resize(max_size);
        rotations.resize(max_size);
        sizes.resize(max_size);
        velocities.resize(max_size);
        colors.resize(max_size);

        num_total = max_size;
        num_alive = 0;
    }

}