#pragma once
#include <core/Component.h>
#include <res/Resource.h>

#include <gfx/vertex_descriptor.h>

namespace idk
{
	class Mesh;
	class Material;

	class MeshRenderer
		: public Component<MeshRenderer>
	{
	public:
		bool enabled { true };

		RscHandle<Mesh>     mesh;
		RscHandle<Material> material;

		bool cast_shadows   { true };
		bool receive_shadows{ true };

		bool IsActiveAndEnabled() const;

		static const renderer_reqs& GetRequiredAttributes();
	};
}