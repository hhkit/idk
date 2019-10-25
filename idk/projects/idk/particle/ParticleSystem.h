#pragma once
#include <idk.h>
#include <core/Component.h>
#include <particle/ParticleSystemModules.h>
#include <particle/ParticleData.h>

namespace idk
{
    class ParticleSystem
        : public Component<ParticleSystem>
    {
    public:
        enum State { Stopped, Playing, Paused };

        MainModule main;
        EmissionModule emission;

        ParticleData data;
        char state = Stopped;
        float time = 0;
        float emitter_clock = 0;

        void Step(float dt);
        void Emit();
    };
}