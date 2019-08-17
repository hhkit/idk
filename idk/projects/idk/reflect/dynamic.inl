#pragma once

#include <reflect/reflect.h>

namespace idk::reflect
{

	struct dynamic::base
	{
		virtual void* get() const = 0;
		virtual uni_container to_container() const = 0;
		virtual vector<dynamic> unpack() const = 0;
		virtual ~base() {}
	};

	template<typename T>
	struct dynamic::derived : dynamic::base
	{
		T obj;
		using DecayedT = std::decay_t<T>;

		template<typename U>
		derived(U&& obj)
			: obj{ std::forward<U>(obj) }
		{}

		void* get() const override
		{
			return const_cast<void*>(static_cast<const void*>(&obj));
		}

		uni_container to_container() const override
		{
			if constexpr (is_sequential_container_v<T> || is_associative_container_v<T>)
				return uni_container{ const_cast<T&>(obj) };
			else
				throw "not a container!";
		}

		vector<dynamic> unpack() const override
		{
			if constexpr (is_template_v<DecayedT, std::pair> || is_template_v<DecayedT, std::tuple>)
			{
				vector<dynamic> vec;
				unpack_helper(vec, std::make_index_sequence<std::tuple_size<DecayedT>::value>());
				return vec;
			}
			else
				throw "not a tuple!";
		}

		template<size_t... Is>
		void unpack_helper(vector<dynamic>& vec, std::index_sequence<Is...>) const
		{
			(vec.push_back(const_cast<std::tuple_element_t<Is, DecayedT>&>(std::get<Is>(obj))), ...);
		}
	};

	class dynamic::property_iterator
	{
	public:
		property_iterator(const dynamic& obj, size_t index = 0);
		property_iterator& operator++(); //prefix increment
		bool operator==(const property_iterator&);
		bool operator!=(const property_iterator&);
		property operator*() const;

	private:
		const dynamic& obj;
		size_t index;
	};

	template<typename T, typename>
	dynamic::dynamic(T&& obj)
		: type{ get_type<T>() }, _ptr{ std::make_shared<derived<T>>(std::forward<T>(obj)) }
	{}

	template<typename T>
	dynamic& dynamic::operator=(const T& rhs)
	{
		assert(is<std::decay_t<T>>());
		type._context->copy_assign(_ptr->get(), &rhs);
		return *this;
	}

	template<typename T>
	bool dynamic::is() const
	{
		return type.hash() == typehash<T>();
	}

	template<typename T>
	T& dynamic::get() const
	{
		return *static_cast<T*>(_ptr->get());
	}

	// recursively visit all members
	// visitor must be a function with signature:
	//  (auto&& key, auto&& value, int depth_change) -> bool/void
	// 
	// key:
	//     name of property (const char*), or
	//     container key when visiting container elements ( K = std::decay_t<decltype(key)> )
	//     for sequential containers, it will be size_t. for associative, it will be type K
	// value:
	//     the value, use T = std::decay_t<decltype(value)> to get the type
	// depth_change: (int)
	//     change in depth. -1 (up a level), 0 (stay same level), or 1 (down a level)
	// 
	// return false to stop recursion. if function doesn't return, it always recurses
	template<typename Visitor>
	void dynamic::visit(Visitor&& visitor) const
	{
		int depth = 0;
		detail::visit(_ptr->get(), type, std::forward<Visitor>(visitor), depth);
	}

}