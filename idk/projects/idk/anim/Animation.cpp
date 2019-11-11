#include "stdafx.h"
#include "Animation.h"
#include <set>

namespace idk::anim
{
	AnimatedBone* Animation::GetAnimatedBone(string_view name)
	{
		auto res = _animated_bones.find(name.data());
		if (res == _animated_bones.end())
			return nullptr;
		return &res->second;
	}

	void Animation::SetSpeeds(float fps, float duration, float num_ticks)
	{
		_fps = fps;
		_duration = duration;
		_num_ticks = num_ticks;
	}

	void Animation::AddAnimatedBone(const AnimatedBone& animated_bone)
	{
		auto res = _animated_bones.find(animated_bone.bone_name);
		if (res == _animated_bones.end())
		{
			_animated_bones.emplace(animated_bone.bone_name, animated_bone);
			
		}
	}
}

