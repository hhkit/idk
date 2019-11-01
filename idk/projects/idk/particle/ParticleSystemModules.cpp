#include "stdafx.h"
#include "ParticleSystemModules.h"
#include <particle/ParticleSystem.h>
#include <math/euler_angles.h>

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
        if (!enabled)
            return;
        vec3 v = gen_dir(sys.rnd);
        auto& data = sys.data;
        data.position[i] = v * (radius - sys.rnd.rangef(0, radius_thickness) * radius);
        data.velocity_start[i] = (randomize_direction ? lerp(v, gen_dir(sys.rnd), randomize_direction) : v) * sys.main.start_speed;
    }

    void VelocityOverLifetimeModule::Update(ParticleSystem& sys, float dt)
    {
        if (!enabled)
            return;
        for (uint16_t i = 0; i < sys.data.num_alive; ++i)
        {
            const float t = sys.data.age[i] / sys.data.lifetime[i];
            const vec3 orb = orbital.Evaluate(t) * dt; // ORRRBBBB
            const vec3 r = sys.data.position[i] - offset.Evaluate(t);
            const vec3 orb_delta = mat3(euler_angles{ rad(orb.x), rad(orb.y), rad(orb.z) }) * r - r;

            sys.data.velocity_delta[i] = linear.Evaluate(t) + orb_delta / dt + radial.Evaluate(t) * sys.data.position[i].get_normalized();
        }
    }

    void ColorOverLifetimeModule::Update(ParticleSystem& sys)
    {
        if (!enabled)
            return;
        for (uint16_t i = 0; i < sys.data.num_alive; ++i)
            sys.data.color[i] = color.Evaluate(sys.data.age[i] / sys.data.lifetime[i]);
    }

    void SizeOverLifetimeModule::Update(ParticleSystem& sys)
    {
        if (!enabled)
            return;
        for (uint16_t i = 0; i < sys.data.num_alive; ++i)
            sys.data.size[i] = size.Evaluate(sys.data.age[i] / sys.data.lifetime[i]);
    }

    void RotationOverLifetimeModule::Update(ParticleSystem& sys, float dt)
    {
        if (!enabled)
            return;
        for (uint16_t i = 0; i < sys.data.num_alive; ++i)
            sys.data.rotation[i] += angular_velocity.Evaluate(sys.data.age[i] / sys.data.lifetime[i]) * dt;
    }

}