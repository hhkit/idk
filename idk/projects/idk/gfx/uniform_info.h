#pragma once
#include <idk.h>

namespace idk
{
	//This is currently unused for now, remove if unused by milestone 1.
	struct uniform_info
	{
		enum UniformType
		{
			eFloat,
			eVec2,
			eVec3,
			eVec4,
			eMat3,
			eMat4,
			eSampler,
		};
		enum UniformStage
		{
			eVertex,
			eFragment,
		};
		struct layout
		{
			UniformStage stage;

		};
		vector<layout> layouts;
		uint32_t TotalSize();
	};
}