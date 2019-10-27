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
        enum State { Awake, Stopped, Playing, Paused };

        MainModule main;
        EmissionModule emission;
        RendererModule renderer;

        ParticleData data;
        float time = 0;
        float emitter_clock = 0;
        vec3 origin;
        char state = Awake;

        void Play();
        void Stop();
        void Step(float dt);
        void Emit();
    };
}