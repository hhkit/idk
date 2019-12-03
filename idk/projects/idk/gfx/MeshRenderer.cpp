#include "stdafx.h"
#include "MeshRenderer.h"
#include <core/GameObject.h>
#include <common/Transform.h>
#include <gfx/GraphicsSystem.h>
#include <gfx/Camera.h>
#include <gfx/Mesh.h>
#include <math/matrix_transforms.h>
#include <phys/RigidBody.h>
#include <core/Scheduler.h>

namespace idk
{
	bool MeshRenderer::IsActiveAndEnabled() const
	{
		return GetGameObject()->ActiveInHierarchy() && enabled;
	}

	RenderObject MeshRenderer::GenerateRenderObject() const
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

		return RenderObject{
			.obj_id            = GetGameObject(),
			.mesh              = mesh,
			.material_instance = material_instance,

			.velocity  = vec3{},
			.transform = tfm,

			.cast_shadows    = cast_shadows,
			.receive_shadows = receive_shadows,

			//.bounding_volume = mesh->bounding_volume,

			.renderer_req = &GetRequiredAttributes(),
			.layer_mask = layer_to_mask(this->GetGameObject()->Layer())
			//{
			//	{0,vtx::Attrib::Position},
			//	{1,vtx::Attrib::Normal},
			//	{2,vtx::Attrib::UV},
			//	{3,vtx::Attrib::Tangent},
			//}
		};
	}

	const renderer_attributes& MeshRenderer::GetRequiredAttributes()
	{
		static renderer_attributes r
		{ {
			std::make_pair(vtx::Attrib::Position, 0),
			std::make_pair(vtx::Attrib::Normal,   1),
			std::make_pair(vtx::Attrib::UV,       2),
			std::make_pair(vtx::Attrib::Tangent,  3),
		},
		{
			std::make_pair(vtx::InstAttrib::ModelTransform, 4),
			std::make_pair(vtx::InstAttrib::Normaltransform,8),
		}
		};
		return r;
	}
}