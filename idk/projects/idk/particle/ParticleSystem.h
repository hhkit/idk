#pragma once
#include <idk.h>
#include <core/Component.h>

namespace idk
{
    struct ParticleModule
    {

    };

    struct MainModule : ParticleModule
    {
        float duration = 5.0f;
    };

    struct EmissionModule : ParticleModule
    {

    };

    struct ParticleUpdater : ParticleModule
    {

    };

    class ParticleSystem
        : public Component<ParticleSystem>
    {
    public:
        bool enabled{ true };


    };
}