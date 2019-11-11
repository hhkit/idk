#include "dual_set.h"
#pragma once

namespace idk
{
	template<typename T1, typename T2>
	typename dual_set<T1, T2>::const_iterator dual_set<T1, T2>::begin() const
	{
		return _table_first.begin();
	}
	template<typename T1, typename T2>
	typename dual_set<T1, T2>::const_iterator dual_set<T1, T2>::end() const
	{
		return _table_first.end();
	}
	template<typename T1, typename T2>
	inline bool dual_set<T1, T2>::empty() const
	{
		return _table_first.empty();
	}
	template<typename T1, typename T2>
	inline size_t dual_set<T1, T2>::size() const
	{
		return _table_first.size();
	}
	template<typename T1, typename T2>
	inline size_t dual_set<T1, T2>::max_size() const
	{
		return _table_first.max_size();
	}
	template<typename T1, typename T2>
	inline void dual_set<T1, T2>::clear()
	{
		_table_first.clear();
		_table_second.clear();
	}
	template<typename T1, typename T2>
	opt<typename dual_set<T1, T2>::const_iterator> dual_set<T1, T2>::insert(const value_type& value)
	{
		auto&& [first, second] = value;

		auto str = _table_second.find(second);
		if (str != _table_second.end())
			return std::nullopt;

		auto&& [itr, success] = _table_first.insert(value);

		if (!success)
			return std::nullopt;

		_table_second.insert(str, typename table_s::value_type{ second, first });
		return itr;
	}
	template<typename T1, typename T2>
	opt<typename dual_set<T1, T2>::const_iterator> dual_set<T1, T2>::insert(value_type&& value)
	{
		auto&& [first, second] = value;

		auto str = _table_second.find(second);
		if (str != _table_second.end())
			return std::nullopt;


		auto&& [itr, success] = _table_first.insert(std::move(value));

		if (!success)
			return std::nullopt;

		_table_second.insert(str, typename table_s::value_type{ second, first });
		return itr;
	}

	template<typename T1, typename T2>
	template<typename ...Args>
	opt<typename dual_set<T1,T2>::const_iterator> dual_set<T1, T2>::emplace(Args&& ... args)
	{
		const auto [itr, success] = _table_first.emplace(std::forward<Args>(args)...);
		if (!success)
			return std::nullopt;

		const auto itr_s = _table_second.find(itr->second);
		if (itr_s != _table_second.end())
		{
			_table_first.erase(itr);
			return std::nullopt;
		}
		
		_table_second.emplace(itr->second, itr->first);
		return itr;
	}

	template<typename T1, typename T2>
	template<typename>
	typename dual_set<T1, T2>::const_iterator dual_set<T1, T2>::find(const T1& key)
	{
		return find_first(key);
	}

	template<typename T1, typename T2>
	template<typename>
	typename dual_set<T1, T2>::const_iterator  dual_set<T1, T2>::find(const T2& key)
	{
		return find_second(key);
	}


	template<typename T1, typename T2>
	typename dual_set<T1, T2>::const_iterator dual_set<T1, T2>::erase(const_iterator pos)
	{
		_table_second.erase(_table_second.find(pos->second));
		return _table_first.erase(pos);
	}

	template<typename T1, typename T2>
	typename dual_set<T1, T2>::const_iterator dual_set<T1, T2>::find_first(const T1& findme)
	{
		return _table_first.find(findme);
	}
	template<typename T1, typename T2>
	typename dual_set<T1, T2>::const_iterator  dual_set<T1, T2>::find_second(const T2& findme)
	{
		const auto itr_s = _table_second.find(findme);
		if (itr_s == _table_second.end())
			return _table_first.end();
		else
			return _table_first.find(itr_s->second);
	}
}