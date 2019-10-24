#pragma once
#include <idk.h>

namespace idk
{
	struct MonoValue;

	using MonoVariant = variant<
		bool, 
		char, 
		short,
		unsigned short,
		int,
		unsigned int,
		long, 
		unsigned long, 
		float, 
		double,
		string,
		Guid,
		vec2,
		vec3,
		vec4,
		vector<MonoValue>>;

	struct MonoValue
	{
		string name;
		MonoVariant data;
	};
}