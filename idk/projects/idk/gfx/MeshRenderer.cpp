#include "stdafx.h"
#include "MeshRenderer.h"
#include <core/GameObject.h>
#include <common/Transform.h>

namespace idk
{
	bool MeshRenderer::IsActiveAndEnabled() const
	{
		return GetGameObject()->ActiveInHierarchy() && enabled;
	}

	RenderObject MeshRenderer::GenerateRenderObject() const
	{
		return RenderObject{
			mesh,
			material_instance,

			vec3{},
			GetGameObject()->GetComponent<Transform>()->LocalMatrix(),

			cast_shadows,
			receive_shadows
		};
	}

	const renderer_reqs& MeshRenderer::GetRequiredAttributes()
	{
		static renderer_reqs r
		{ {
			std::make_pair(vtx::Attrib::Position, 0),
			std::make_pair(vtx::Attrib::Normal,   1),
			std::make_pair(vtx::Attrib::Tangent,  2),
			std::make_pair(vtx::Attrib::UV,       3),
		} };
		return r;
	}
}