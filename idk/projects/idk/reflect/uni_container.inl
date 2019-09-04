#pragma once

#include "reflect.h"

namespace idk::reflect
{
    template<typename T, typename = void>
    struct has_push_back : std::false_type {};
    template<typename T>
    struct has_push_back<T, std::void_t<decltype(std::declval<T>().push_back(std::declval<T::value_type>()))>>
        : std::true_type {};

    template<typename T, typename = void>
    struct has_insert : std::false_type {};
    template<typename T>
    struct has_insert<T, std::void_t<decltype(std::declval<T>().insert(std::declval<T::value_type>()))>>
        : std::true_type {};

    template<typename T, typename = void>
    struct has_clear : std::false_type {};
    template<typename T>
    struct has_clear<T, std::void_t<decltype(std::declval<T>().clear())>>
        : std::true_type {};

    template<typename T, typename K, typename = void>
    struct has_subscript : std::false_type {};
    template<typename T, typename K>
    struct has_subscript<T, K, std::void_t<decltype(std::declval<T>()[std::declval<K>()])>>
        : std::true_type {};



	template<typename T, typename>
	uni_container::uni_container(T&& obj)
		: type{ get_type<T>() }
		, value_type{ get_type<std::decay_t<T>::value_type>() }
		, _ptr{ std::make_shared<derived<T>>(std::forward<T>(obj)) }
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
        virtual dynamic subscript(size_t index) = 0;
        virtual dynamic subscript(const dynamic& key) = 0;
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
            if constexpr (has_push_back<DecayedT>::value)
            {
                if constexpr (std::is_same_v<DecayedT::value_type, dynamic>)
                    container.push_back(obj);
                else
                    container.push_back(obj.get<DecayedT::value_type>());
            }
            else if constexpr (has_insert<DecayedT>::value)
            {
                if constexpr (std::is_same_v<DecayedT::value_type, dynamic>)
                    container.insert(obj);
                else
                    container.insert(obj.get<DecayedT::value_type>());
            }
			else
				throw "no add method found"; // impl more add methods
		}

		void clear() override
		{
			if constexpr (has_clear<DecayedT>::value)
				container.clear();
			else
				throw "no clear method found";
		}

		size_t size()
		{
			return container.size();
		}

        virtual dynamic subscript(size_t index)
        {
            if constexpr (has_subscript<DecayedT, size_t>::value)
                return container[index];
            else
                throw "cannot subscript with size_t";
        }

        virtual dynamic subscript(const dynamic& key)
        {
            if constexpr (is_associative_container_v<T>)
                return container[key.get<DecayedT::key_type>()];
            else
                return subscript(key.get<size_t>());
        }
	};
}