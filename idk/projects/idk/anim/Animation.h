#pragma once

#include <idk.h>
#include <res/Resource.h>

namespace idk::anim
{
	struct AnimationMeta
	{

	};

	class Animation 
		: public Resource<Animation>
		//, public MetaTag<AnimationMeta>
	{
	public:
		// Key represents a keyframe. Can be translate, rotate, or scale.
		template<typename T>
		struct Key
		{
			Key() = default;
			Key(const T& val, float time) : _val{ val }, _time{ time } {}
			T		_val;
			float	_time;
		};

		struct AnimNode
		{
			string _name;

			vector<Key<vec3>> _translate;
			vector<Key<vec3>> _scale;
			vector<Key<quat>> _rotation;
		};

		string_view GetName()		const { return _name; }
		float		GetFPS()		const { return _fps; }
		float		GetDuration()	const { return _duration; }
		float		GetNumTicks()	const { return _num_ticks; }
		AnimNode*	GetAnimNode(string_view name);

		void SetSpeeds(float fps = 25.0f, float duration = 0.0f, float num_ticks = 0.0f);
		void SetName(string_view name) { _name = name; }
		void AddAnimNode(const AnimNode& node);

	private:
		hash_table<string, size_t> _anim_node_table;
		vector<AnimNode> _anim_nodes;

		string _name;

		float _fps		 = 25.0f;
		float _duration  = 0.0f;
		float _num_ticks = 0.0f;
	};
}