#pragma once
#include "../idk.h"

namespace idk
{
	template <typename T>
	class ObjectHandle;

	template <typename T>
	class ObjectPool
	{
	public:
		using Handle = idk::ObjectHandle<T>;
		static constexpr auto type_id = Handle::type_id;

		ObjectPool(size_t reserve = 8192);

		// accessors
		T* at(const Handle&) const;
		T* data() const;

		// modifiers
		Handle emplace();
		Handle emplace_at(const Handle&);
		bool   remove(const Handle&);
		//bool   remove(T& removeme);

		// operator overloads
		T&     operator[](const Handle&) const;
	private:
		using index_t = unsigned;
		static constexpr auto invalid = index_t{ 0xFFFFFFFF };

		struct HandleInflect
		{
			index_t  intern_index = invalid;
			uint16_t uses         = index_t{};
		};

		vector<HandleInflect> lookup;
		vector<byte>          intern;

		index_t first_free = 0;
		index_t in_use     = 0;
		index_t freelist   = invalid;

		void shift_first_free();
		void grow();
	};
}

#include "ObjectPool.inl"