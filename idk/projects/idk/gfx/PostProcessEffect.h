#pragma once
#include <core/Core.h>

namespace idk {
	struct PostProcessEffect
	{
		//Fog
		alignas(16) vec3 fogColor = vec3(0.5, 0.5, 0.5);
		real FogDensity = 0.001f;

		//Bloom
		real blurStrength = 1.5f;
		real blurScale = 2.f;

		alignas(4) int useFog = 1;
		alignas(4) int useBloom = 1;
	};
};