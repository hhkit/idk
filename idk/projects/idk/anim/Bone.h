#pragma once
#include <core/Component.h>


namespace idk
{
	struct Bone
		: public Component<Bone>
	{
		string _bone_name;
		int _bone_index = -1;
		float weight_left = 1.0f;
	};
}