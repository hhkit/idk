#include "stdafx.h"
#include "MeshRenderer.h"
#include <core/GameObject.h>
#include <common/Transform.h>
#include <gfx/GraphicsSystem.h>
#include <gfx/Camera.h>

namespace idk
{
	bool MeshRenderer::IsActiveAndEnabled() const
	{
		return GetGameObject()->ActiveInHierarchy() && enabled;
	}

	RenderObject MeshRenderer::GenerateRenderObject() const
	{
		return RenderObject{
			GetGameObject(),
			mesh,
			material_instance,

			vec3{},
			GetGameObject()->GetComponent<Transform>()->GlobalMatrix() ,

			cast_shadows,
			receive_shadows,
			&GetRequiredAttributes()
			//{
			//	{0,vtx::Attrib::Position},
			//	{1,vtx::Attrib::Normal},
			//	{2,vtx::Attrib::UV},
			//	{3,vtx::Attrib::Tangent},
			//}
		};
	}

	const renderer_reqs& MeshRenderer::GetRequiredAttributes()
	{
		static renderer_reqs r
		{ {
			std::make_pair(vtx::Attrib::Position, 0),
			std::make_pair(vtx::Attrib::Normal,   1),
			std::make_pair(vtx::Attrib::UV,       2),
			std::make_pair(vtx::Attrib::Tangent,  3),
		} };
		return r;
	}
}