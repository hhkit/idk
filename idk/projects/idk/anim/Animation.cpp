#include "stdafx.h"
#include "Animation.h"


namespace idk::anim
{
	Animation::AnimNode* Animation::GetAnimNode(string_view name)
	{
		auto res = _anim_node_table.find(name.data());
		if (res == _anim_node_table.end())
			return nullptr;
		return &res->second;
	}

	Animation::EasyAnimNode* Animation::GetEasyAnimNode(string_view name)
	{
		auto res = _easy_anim_table.find(name.data());
		if (res == _easy_anim_table.end())
			return nullptr;
		return &res->second;
	}

	void Animation::SetSpeeds(float fps, float duration, float num_ticks)
	{
		_fps = fps;
		_duration = duration;
		_num_ticks = num_ticks;
	}

	void Animation::AddAnimNode(const AnimNode& node)
	{
		UNREFERENCED_PARAMETER(node);
		// auto res = _anim_node_table.find(node._name);
		// if (res == _anim_node_table.end())
		// {
		// 	_anim_nodes.emplace_back(node);
		// 	_anim_node_table.emplace(_anim_nodes.back()._name, _anim_nodes.size() - 1);
		// }
	}
	void Animation::AddEasyAnimNode(const EasyAnimNode& easy_node)
	{
		auto res = _anim_node_table.find(easy_node._name);
		if (res == _anim_node_table.end())
		{
			_easy_anim_table.emplace(easy_node._name, easy_node);
			
		}
	}
}

