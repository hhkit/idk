#include "stdafx.h"
#include "SkinnedMeshRenderer.h"
#include <core/GameObject.h>
#include <common/Transform.h>
#include <gfx/GraphicsSystem.h>
#include <gfx/Camera.h>

namespace idk
{
	bool SkinnedMeshRenderer::IsActiveAndEnabled() const
	{
		return GetGameObject()->ActiveInHierarchy() && enabled;
	}
	AnimatedRenderObject SkinnedMeshRenderer::GenerateRenderObject() const
	{
		return AnimatedRenderObject{
			mesh,
			material_instance,

			vec3{},
			GetGameObject()->GetComponent<Transform>()->GlobalMatrix() ,

			cast_shadows,
			receive_shadows,
			   &GetRequiredAttributes()
			 //{{0,vtx::Attrib::Position},
			 //{1,vtx::Attrib::Normal},
			 //{2,vtx::Attrib::UV},
			 //{3,vtx::Attrib::Tangent},
			 //{4,vtx::Attrib::BoneID},
			 //{5,vtx::Attrib::BoneWeight}}
		};
	}
	const renderer_reqs& SkinnedMeshRenderer::GetRequiredAttributes()
	{
		static renderer_reqs r
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