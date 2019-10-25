#include "stdafx.h"
#include "ParticleSystemUpdater.h"
#include <particle/ParticleSystem.h>

namespace idk
{

    void ParticleSystemUpdater::Update(span<ParticleSystem> span_ps)
    {
        const float dt = Core::GetDT().count();
        for (auto& ps : span_ps)
        {
            ps.Step(dt);
        }
    }

}
