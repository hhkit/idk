#pragma once
#include <idk.h>
#include <core/Component.h>
#include <particle/ParticleSystemModules.h>
#include <particle/ParticleData.h>
#include <math/matrix_decomposition.h>
#include <math/random.h>

namespace idk
{
    class ParticleSystem
        : public Component<ParticleSystem>
    {
    public:
        enum State { Awake, Stopped, Playing, Paused };

        MainModule main;
        EmissionModule emission;
        ShapeModule shape;
        VelocityOverLifetimeModule velocity_over_lifetime;
        ColorOverLifetimeModule color_over_lifetime;
        SizeOverLifetimeModule size_over_lifetime;
        RotationOverLifetimeModule rotation_over_lifetime;
        RendererModule renderer;

        ParticleData data;
        random rnd;
        float time = 0;
        float cycle_time = 0;
        matrix_decomposition<real> transform;
        char state = Awake;

        void Play();
        void Pause();
        void Stop();
        void Step(float dt);
        void Emit(float age = 0);
    };
}