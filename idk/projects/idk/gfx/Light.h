#pragma once
#include <idk.h>
#include <core/Component.h>
#include <gfx/LightTypes.h>

namespace idk
{
	class Light
		: public Component<Light>
	{
	public:
		LightVariant light;
		real         shadow_bias   { epsilon };
		bool         casts_shadows { true };
	};
}