#pragma once
#include <core/Component.h>
#include <res/Resource.h>

#include <gfx/vertex_descriptor.h>
#include <gfx/RenderObject.h>

namespace idk
{
	class Mesh;
	class Material;
	class MaterialInstance;

	class MeshRenderer
		: public Component<MeshRenderer>
	{
	public:
		bool enabled { true };

		RscHandle<Mesh>            mesh;
		RscHandle<MaterialInstance> material_instance;

		bool cast_shadows   { true };
		bool receive_shadows{ true };

		bool IsActiveAndEnabled() const;
		RenderObject GenerateRenderObject() const;

		static const renderer_attributes& GetRequiredAttributes();
	};
}