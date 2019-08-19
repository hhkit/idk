#include "stdafx.h"
#include "MeshRenderer.h"
#include <core/GameObject.h>

namespace idk
{
	bool MeshRenderer::IsActiveAndEnabled() const
	{
		return GetGameObject()->GetActiveInHierarchy() && enabled;
	}

	const renderer_reqs& MeshRenderer::GetRequiredAttributes()
	{
		static renderer_reqs r
		{ {
			attrib_meta{0, vtx::Attrib::Position},
			attrib_meta{1, vtx::Attrib::Normal},
			attrib_meta{2, vtx::Attrib::Tangent},
			attrib_meta{3, vtx::Attrib::UV},
		} };
		return r;
	}
}