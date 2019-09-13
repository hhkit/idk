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

		struct Channel
		{
			string _name;
			mat4 _node_transform;
			bool _is_animated = false;
			vector<Key<vec3>> _translate{};
			vector<Key<vec3>> _scale{};
			vector<Key<quat>> _rotation{};
		};

		// represents an animated bone
		struct AnimNode
		{
			string _name;

			mat4 GetTransform(float time) const;

			vector<Channel> _channels;
		};

		struct EasyAnimNode
		{
			string _name;
			bool _debug_assert = false;
			vector<Channel> _channels;
		};

		string_view GetName()		const { return _name; }
		float		GetFPS()		const { return _fps; }
		float		GetDuration()	const { return _duration; }
		float		GetNumTicks()	const { return _num_ticks; }
		AnimNode*	GetAnimNode(string_view name);
		EasyAnimNode* GetEasyAnimNode(string_view name);
		const hash_table<string, EasyAnimNode>& data() { return _easy_anim_table; }

		void SetSpeeds(float fps = 25.0f, float duration = 0.0f, float num_ticks = 0.0f);
		void SetName(string_view name) { _name = name; }
		void AddAnimNode(const AnimNode& node);
		void AddEasyAnimNode(const EasyAnimNode& easy_node);
	private:
		hash_table<string_view, AnimNode> _anim_node_table;
		hash_table<string, EasyAnimNode> _easy_anim_table;
		string _name;

		float _fps		 = 25.0f;
		float _duration  = 0.0f;
		float _num_ticks = 0.0f;
	};
}