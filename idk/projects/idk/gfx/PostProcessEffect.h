#pragma once
#include <core/Core.h>

namespace idk {
	struct PostProcessEffect
	{
		//Fog
		alignas(16) vec3 threshold = vec3(0.1, 0.9, 0.9);
		alignas(16) vec3 fogColor = vec3(0.5, 0.5, 0.5);
		real FogDensity = 0.001f;

		//Bloom
		real blurStrength = 1.2f;
		real blurScale = 2.f;

		alignas(4) bool useFog = 1;
		alignas(4) bool useBloom = 1;
	};
};