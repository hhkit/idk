#include "stdafx.h"
#include "SkinnedMeshRenderer.h"
#include <core/GameObject.h>
#include <common/Transform.h>
#include <gfx/GraphicsSystem.h>
#include <gfx/Camera.h>
#include <math/matrix_transforms.h>
#include <phys/RigidBody.h>
#include <core/Scheduler.h>

namespace idk
{
	bool SkinnedMeshRenderer::IsActiveAndEnabled() const
	{
		return GetGameObject()->ActiveInHierarchy() && enabled;
	}
	AnimatedRenderObject SkinnedMeshRenderer::GenerateRenderObject() const
	{
		auto tfm = GetGameObject()->GetComponent<Transform>()->GlobalMatrix();
		Handle<RigidBody> rb;
		auto search = GetGameObject();
		while (search)
		{
			rb = GetGameObject()->GetComponent<RigidBody>();
			if (rb)
				break;
			else
				search = search->Parent();
		}
		if (rb)
			tfm[3] += vec4{ rb->velocity() * Core::GetScheduler().GetRemainingTime().count(), 0 };
		return AnimatedRenderObject{
			RenderObject
			{
			.obj_id = GetGameObject(),
			.mesh = mesh,
			.material_instance = material_instance,

			.velocity = vec3{},
			.transform = tfm,

			.cast_shadows = cast_shadows,
			.receive_shadows = receive_shadows,
			.renderer_req = &GetRequiredAttributes(),
			.layer_mask = layer_to_mask(GetGameObject()->Layer())
			}
		};
	}
	const renderer_attributes& SkinnedMeshRenderer::GetRequiredAttributes()
	{
		static renderer_attributes r
		{ {
			std::make_pair(vtx::Attrib::Position,	0),
			std::make_pair(vtx::Attrib::Normal,		1),
			std::make_pair(vtx::Attrib::UV,			2),
			std::make_pair(vtx::Attrib::Tangent,	3),
			std::make_pair(vtx::Attrib::BoneID,		4),
			std::make_pair(vtx::Attrib::BoneWeight,	5),
		} };
		return r;
	}
}