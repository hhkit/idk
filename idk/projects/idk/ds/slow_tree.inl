#include "slow_tree.h"
#pragma once

namespace idk
{
	template<typename T>
	template<typename ...Args, typename>
	inline slow_tree<T>::slow_tree(Args&& ...args)
		: obj{std::forward<Args>(args)...}
	{
	}
	template<typename T>
	template<typename Visitor>
	inline void slow_tree<T>::visit(Visitor&& visitor) const
	{
		visit(policy::pre_order, std::forward<Visitor>(visitor));
	}
	template<typename T>
	template<typename Visitor>
	inline void slow_tree<T>::visit(policy::pre_order_t policy, Visitor&& visitor) const
	{
		int depth = 0;
		int last_visit_depth = 0;
		visit_impl(policy, std::forward<Visitor>(visitor), depth, last_visit_depth);
	}

	template<typename T>
	template<typename ...Args>
	inline slow_tree<T>& slow_tree<T>::emplace_child(Args&& ... args)
	{
		_children.emplace_front(slow_tree{std::forward<Args>(args)...});
		return _children.front();
	}

	template<typename T>
	template<typename Visitor>
	inline void slow_tree<T>::visit_impl(policy::pre_order_t policy, Visitor&& visitor, int& depth, int& last_visit_depth) const
	{
		int depth_change = depth - last_visit_depth;
		last_visit_depth = depth;

		bool recurse = true;
		if constexpr (std::is_same_v<std::decay_t<decltype(visitor(obj, depth_change))>, void>)
			visitor(obj, depth_change);
		else
		{
			static_assert(std::is_same_v<std::decay_t<decltype(visitor(obj, depth_change))>, bool>, "visitor of tree must be either void or bool");
			recurse &= visitor(obj, depth_change);
		}

		if (recurse)
		{
		++depth;
			for (auto& elem : *this)
				elem.visit_impl(policy, std::forward<Visitor>(visitor), depth, last_visit_depth);
		--depth;
		}
	}

	template<typename T>
	typename slow_tree<T>::iterator slow_tree<T>::begin()
	{
		return _children.begin();
	}

	template<typename T>
	typename slow_tree<T>::iterator slow_tree<T>::end()
	{
		return _children.end();
	}

	template<typename T>
	typename slow_tree<T>::const_iterator slow_tree<T>::begin() const
	{
		return _children.begin();
	}

	template<typename T>
	typename slow_tree<T>::const_iterator slow_tree<T>::end() const
	{
		return _children.end();
	}

	template<typename T>
	inline size_t slow_tree<T>::size() const
	{
		return std::distance(_children.begin(), _children.end());
	}

	template<typename T>
	inline void slow_tree<T>::clear()
	{
		_children.clear();
	}

	template<typename T>
	inline bool slow_tree<T>::pop_child(const T& removeme)
	{
		auto itr = std::find_if(_children.begin(), _children.end(), [](const auto& elem) { return elem.obj == removeme; });
		if (itr != _children.end())
		{
			_children.erase(itr);
			return true;
		}
		return false;
	}
}