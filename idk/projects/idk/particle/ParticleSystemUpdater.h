#pragma once

#include <core/ISystem.h>

namespace idk
{
    class ParticleSystemUpdater : public ISystem
    {
    public:
        virtual void Init() {};
        virtual void Shutdown() {};
        virtual void Update(span<class ParticleSystem>);
    };
}