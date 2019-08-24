#pragma once
#include <idk.h>
#include <meta/meta.h>
#include <ds/span.h>

namespace idk
{
	template <typename T>
	struct pool
	{
	public:
		using iterator_t = T *;
		pool(size_t reserve = 8192);
		~pool();

		// iteration
		iterator_t begin();
		iterator_t end();
		span<T>    span();
		T&         front();
		T&         back();

		// accessors
		T*         data();
		size_t     size() const;
		T&         operator[](const size_t& index);

		// modifiers
		void       reserve(size_t);
		void       grow();
		template<typename ... Args>
		size_t     emplace_back(Args&&...);
		void       pop_back();

	private:
		vector<byte> _intern;
		size_t       _size = 0;
	};
}

#include "pool.inl"