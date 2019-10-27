#pragma once
#include <idk.h>
#include <core/Component.h>
#include <particle/ParticleSystemModules.h>
#include <particle/ParticleData.h>
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
        RendererModule renderer;

        ParticleData data;
        random rnd;
        float time = 0;
        float emitter_clock = 0;
        vec3 origin;
        quat rotation;
        vec3 scale;
        char state = Awake;

        void Play();
        void Stop();
        void Step(float dt);
        void Emit();
    };
}