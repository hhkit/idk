#pragma once
#include <core/Core.h>

namespace idk {
	struct PostProcessEffect
	{
		//Fog
		vec3 threshold = vec3(0.1, 0.9, 0.9);
		color fogColor = color(0.5, 0.5, 0.5,1);
		real fogDensity = 1.8f;


		//Bloom
		real blurStrength = 1.2f;
		real blurScale = 2.f;

		bool useFog = true;
		bool useBloom = true;
	};

	struct PostProcessEffectData {
		//Fog
		alignas(16) vec3 threshold = vec3(0.1, 0.9, 0.9);
		alignas(16) color fogColor = color(0.5, 0.5, 0.5, 1);
		real fogDensity = 1.8f;


		//Bloom
		real blurStrength = 1.2f;
		real blurScale = 2.f;

		alignas(4) int useFog = 1;
		alignas(4) int useBloom = 1;
	};
};