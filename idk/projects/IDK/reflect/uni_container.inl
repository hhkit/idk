#pragma once

#include "reflect.h"

namespace idk::reflect
{

	template<typename T, typename>
	uni_container::uni_container(T&& obj)
		: type{ get_type<T>() }
		, value_type{ get_type<std::decay_t<T>::value_type>() }
		, ptr_{ std::make_shared<derived<T>>(std::forward<T>(obj)) }
	{}


	class uni_container::iterator
	{
	public:
		iterator(const iterator& other);
		iterator(base* base, dynamic iter);
		iterator& operator=(const iterator& other);
		iterator& operator++(); //prefix increment
		bool operator==(const iterator& other) const;
		bool operator!=(const iterator& other) const;
		dynamic operator*() const;

	private:
		dynamic iter;
		uni_container::base* base;

		template<typename T> friend struct derived;
	};


	struct uni_container::base
	{
		virtual iterator begin() = 0;
		virtual iterator end() = 0;
		virtual void iter_inc(iterator&) = 0;
		virtual bool iter_cmp(const iterator& lhs, const iterator& rhs) = 0;
		virtual dynamic iter_deref(const iterator& iter) = 0;
		virtual void add(const dynamic&) = 0;
		virtual void clear() = 0;
		virtual size_t size() = 0;
		virtual ~base() {}
	};

	template<typename T>
	struct uni_container::derived : base
	{
		using DecayedT = std::decay_t<T>;
		T container;

		template<typename U>
		derived(U&& obj)
			: container{ std::forward<U>(obj) }
		{}

		iterator begin() override
		{
			return iterator{ this, container.begin() };
		}

		iterator end() override
		{
			return iterator{ this, container.end() };
		}

		void iter_inc(iterator& iter) override
		{
			++iter.iter.get<DecayedT::iterator>();
		}

		bool iter_cmp(const iterator& lhs, const iterator& rhs) override
		{
			return lhs.iter.get<DecayedT::iterator>() == rhs.iter.get<DecayedT::iterator>();
		}

		dynamic iter_deref(const iterator& iter) override
		{
			return *iter.iter.get<DecayedT::iterator>();
		}

		void add(const dynamic& obj) override
		{
			if constexpr (std::is_same_v<decltype(has_push_back<DecayedT>(0)), std::true_type>)
				container.push_back(obj.get<DecayedT::value_type>());
			else if constexpr (std::is_same_v<decltype(has_insert<DecayedT>(0)), std::true_type>)
				container.insert(obj.get<DecayedT::value_type>());
			else
				throw "no add method found"; // impl more add methods
		}

		void clear() override
		{
			if constexpr (std::is_same_v<decltype(has_clear<DecayedT>(0)), std::true_type>)
				container.clear();
			else
				throw "no clear method found";
		}

		size_t size()
		{
			return container.size();
		}

	private:
		template<typename C, typename = decltype(std::declval<C>().push_back(std::declval<C::value_type>()))>
		constexpr std::true_type has_push_back(int) {}
		template<typename C>
		constexpr std::false_type has_push_back(...) {}

		template<typename C, typename = decltype(std::declval<C>().insert(std::declval<C::value_type>()))>
		constexpr std::true_type has_insert(int) {}
		template<typename C>
		constexpr std::false_type has_insert(...) {}

		template<typename C, typename = decltype(std::declval<C>().clear())>
		constexpr std::true_type has_clear(int) {}
		template<typename C>
		constexpr std::false_type has_clear(...) {}
	};
}