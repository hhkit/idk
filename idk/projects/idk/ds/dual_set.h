#pragma once
#include <utility>

#include <idk.h>

namespace idk
{
	template<typename T1, typename T2>
	class dual_set
	{
		using table_p = hash_table<T1, T2>;
		using table_s = hash_table<T2, T1>;
	public:
		using iterator = typename table_p::iterator;
		using const_iterator = typename table_p::const_iterator;
		using value_type = typename table_p::value_type;

		// constructors
		dual_set() = default;

		// iterators
		const_iterator begin() const;
		const_iterator end() const;

		// capacity
		bool   empty() const;
		size_t size() const;
		size_t max_size() const;

		// modifiers
		void clear();
		opt<const_iterator> insert(const value_type& value);
		opt<const_iterator> insert(value_type&& value);
		template<typename ... Args>
		opt<const_iterator> emplace(Args&&...);

		const_iterator erase(const_iterator pos);
	
		// lookup
		template<typename = std::enable_if_t<!std::is_same_v<T1, T2>>>
		const_iterator find(const T1&);
		template<typename = std::enable_if_t<!std::is_same_v<T1, T2>>>
		const_iterator find(const T2&);
		const_iterator find_first(const T1&);
		const_iterator find_second(const T2&);
	private:
		table_p _table_first;
		table_s _table_second;
	};
}