#include "stdafx.h"
#include "ParticleSystemUpdater.h"
#include <particle/ParticleSystem.h>
#include <core/GameObject.h>
#include <common/Transform.h>
#include <gfx/MaterialInstance.h>

namespace idk
{

    void ParticleSystemUpdater::Init()
    {
    }

    void ParticleSystemUpdater::LateInit()
    {
        auto particle_frag = Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/default_particle.frag", false);
        auto particle_mat = Core::GetResourceManager().Create<Material>(Guid{ 0xcfa6c3e0, 0x9b52, 0x412b, 0xb3e7c85ee93190ca });
        particle_mat->_shader_program = particle_frag;
        particle_mat->Name("Default Particle");
        auto particle_mat_inst = Core::GetResourceManager().Create<MaterialInstance>(RendererModule::default_material_inst);
        particle_mat_inst->material = particle_mat;
        particle_mat_inst->Name("Default Particle");
        particle_mat->_default_instance = particle_mat_inst;

        particle_mat->uniforms.emplace("Texture", UniformInstance{ "_uTex[0]",
            *Core::GetResourceManager().Load<Texture>("/engine_data/textures/default_particle.png", false) });
    }

    void ParticleSystemUpdater::Update(span<ParticleSystem> span_ps)
    {
        const float dt = Core::GetRealDT().count();
        for (auto& ps : span_ps)
        {
            if (ps.state >= ParticleSystem::Playing)
                ps.transform = decompose(ps.GetGameObject()->GetComponent<Transform>()->GlobalMatrix());
            ps.Step(dt);
        }
    }

    void ParticleSystemUpdater::EditorUpdate(span<class ParticleSystem> span_ps)
    {
        const float dt = Core::GetRealDT().count();
        for (auto& ps : span_ps)
        {
            if (ps.state >= ParticleSystem::Playing)
            {
                ps.transform = decompose(ps.GetGameObject()->GetComponent<Transform>()->GlobalMatrix());
                ps.main.destroy_on_finish = false;
                ps.Step(dt);
                ps.main.destroy_on_finish = true;
            }
        }
    }

}
