#pragma once

#include <idk.h>
#include <res/Resource.h>

namespace idk::anim
{
	struct AnimationMeta
	{

	};

	// KeyFrame represents a keyframe. Can be translate, rotate, or scale.
	template<typename T>
	struct KeyFrame
	{
		KeyFrame() = default;
		KeyFrame(const T& val, float time) : val{ val }, time{ time } {}
		T		val;
		float	time;
	};

	struct AnimatedBone
	{
		string bone_name;
		
		vector<KeyFrame<vec3>> translate_track{};
		vector<KeyFrame<vec3>> scale_track{};
		vector<KeyFrame<quat>> rotation_track{};
	};

	struct EasyAnimNode
	{
		string _name;
		bool _debug_assert = false;
		vector<AnimatedBone> _channels;
	};

	class Animation 
		: public Resource<Animation>
		//, public MetaTag<AnimationMeta>
	{
	public:
		float		GetFPS()		const { return _fps; }
		float		GetDuration()	const { return _duration; }
		float		GetNumTicks()	const { return _num_ticks; }

		AnimatedBone* GetAnimatedBone(string_view name);
		const hash_table<string, AnimatedBone>& data() { return _animated_bones; }

		void SetSpeeds(float fps = 25.0f, float duration = 0.0f, float num_ticks = 0.0f);

		void AddAnimatedBone(const AnimatedBone& animated_bone);
	private:
		hash_table<string, AnimatedBone> _animated_bones;

		float _fps		 = 25.0f;
		float _duration  = 0.0f;
		float _num_ticks = 0.0f;
	};
}