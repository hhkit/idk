#include "stdafx.h"
#include "ParticleSystemModules.h"
#include <particle/ParticleSystem.h>

namespace idk
{

    static vec3 gen_dir(random& rnd)
    {
        vec3 v{ rnd.gaussf(0, 1.0f), rnd.gaussf(0, 1.0f), rnd.gaussf(0, 1.0f) };
        auto sqr_mag = v.length_sq();
        if (sqr_mag > epsilon)
            return v / sqrtf(sqr_mag);
        else
            return gen_dir(rnd);
    }

    void ShapeModule::Generate(ParticleSystem& sys, uint16_t i)
    {
        vec3 v = gen_dir(sys.rnd);
        auto& data = sys.data;
        data.positions[i] = v * (radius - sys.rnd.rangef(0, radius_thickness));
        data.velocities[i] = (randomize_direction ? lerp(v, gen_dir(sys.rnd), randomize_direction) : v) * sys.main.start_speed;
    }

}