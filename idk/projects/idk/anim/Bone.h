#pragma once
#include <core/Component.h>


namespace idk
{
	struct Bone
		: public Component<Bone>
	{
		string bone_name;
		int bone_index = -1;
	};
}