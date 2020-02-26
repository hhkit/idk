#include "stdafx.h"
#include "ParticleSystemUpdater.h"
#include <particle/ParticleSystem.h>
#include <core/GameObject.inl>
#include <common/Transform.h>
#include <gfx/MaterialInstance.h>
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>
#include <ds/span.inl>
#include <ds/result.inl>

namespace idk
{

    void ParticleSystemUpdater::Init()
    {
    }

    void ParticleSystemUpdater::LateInit()
    {
        auto res = Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/default_particle.frag", false);
        if (!res)
            return;

        auto particle_frag = *res;
        auto particle_mat = Core::GetResourceManager().LoaderCreateResource<Material>(Guid{ 0xcfa6c3e0, 0x9b52, 0x412b, 0xb3e7c85ee93190ca });
        if (particle_mat)
        {
            particle_mat->_shader_program = particle_frag;
            particle_mat->Name("Default Particle");
        }
        auto particle_mat_inst = Core::GetResourceManager().LoaderCreateResource<MaterialInstance>(RendererModule::default_material_inst);
        if (particle_mat_inst)
        {
            particle_mat_inst->material = particle_mat;
            particle_mat_inst->Name("Default Particle");
            particle_mat->_default_instance = particle_mat_inst;

            particle_mat->uniforms.emplace("Texture", UniformInstance{ "_uTex[0]",
                *Core::GetResourceManager().Load<Texture>("/engine_data/textures/default_particle.png", false) });
        }
    }

    void ParticleSystemUpdater::Update(span<ParticleSystem> span_ps)
    {
        const float dt = Core::GetRealDT().count();
        for (auto& ps : span_ps)
        {
            if (ps.state >= ParticleSystem::Playing)
                ps.transform = ps.GetGameObject()->GetComponent<Transform>()->GlobalMatrix();
            ps.Step(dt);
        }
    }

}
