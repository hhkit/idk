#include "stdafx.h"
#include "ParticleData.h"

namespace idk
{

    void ParticleData::Allocate(uint16_t max_size)
    {
        positions.reset(new vec3[max_size]);
        rotations.reset(new float[max_size]);
        sizes.reset(new float[max_size]);
        velocities.reset(new vec3[max_size]);
        num_total = max_size;
        num_alive = 0;
    }

}