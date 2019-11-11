#pragma once
#include <idk.h>
#include <core/Component.h>

#include <res/Resource.h>

#include <gfx/vertex_descriptor.h>
#include <gfx/RenderObject.h>
#include <gfx/MaterialInstance.h>

namespace idk
{
	class SkinnedMeshRenderer
		: public Component<SkinnedMeshRenderer>
	{
	public:
		bool enabled{ true };

		RscHandle<Mesh>             mesh;
		RscHandle<MaterialInstance> material_instance;

		bool cast_shadows{ true };
		bool receive_shadows{ true };

		bool IsActiveAndEnabled() const;
		AnimatedRenderObject GenerateRenderObject() const;

		static const renderer_attributes& GetRequiredAttributes();
	};
}