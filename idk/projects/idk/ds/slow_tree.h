#pragma once
#include <idk.h>
#include <forward_list>
namespace idk
{
	namespace policy
	{
		struct post_order_t {};
		struct pre_order_t {};
		struct level_order_t {};

		constexpr post_order_t  post_order;
		constexpr pre_order_t   pre_order;
		constexpr level_order_t level_order;
	}

	template<typename T>
	class slow_tree
	{
		using intern = std::forward_list<slow_tree>;
	public:
		using iterator = typename intern::iterator;
		using const_iterator = typename intern::const_iterator;
		T obj;

		// constructors
		template<typename ... Args,
			typename = sfinae<!(std::is_same_v<std::decay_t<Args>, slow_tree> || ...)>>
		slow_tree(Args && ... args);

		// accessors
		// visitor must be of type void(<T>, int depth). 
		// depth will be +1 if it descends one level, 0 if it stays and -1 if it jumps up the hierarchy
		template<typename Visitor> void visit(Visitor&& visitor) const;
		template<typename Visitor> void visit(policy::pre_order_t, Visitor&& visitor) const;

		// iterators
		iterator begin();
		iterator end();
		const_iterator begin() const;
		const_iterator end()   const;
		size_t size() const;

		// modifiers
		void clear();
		template<typename ...Args>
		slow_tree& emplace_child(Args&& ...);

		bool pop_child(const T& removeme);
		template<typename Pred, typename = sfinae<std::is_invocable_v<Pred, T>>> int pop_child(Pred&& predicate);
	private:
		std::forward_list<slow_tree> _children;

		template<typename Visitor> void visit_impl(policy::pre_order_t, Visitor&& visitor, int& depth, int& last_depth) const;
	};
}

#include "slow_tree.inl"