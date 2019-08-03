#pragma once
#include <idk.h>
#include <util/meta.h>

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
		~ObjectPool();

		// accessors
		T*   at(const Handle&) const;
		T*   data() const;
		bool validate(const Handle&) const;

		// modifiers
		Handle emplace();
		Handle emplace_at(const Handle&);
		bool   remove(const Handle&);
		bool   remove(T& removeme);

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

		index_t first_free_handle  = 0;
		index_t pool_size          = 0;

		void shift_first_free();
		void grow();
		tuple<T&, index_t> create();

		// non-copiable non-movable
		ObjectPool(const ObjectPool&) = delete;
		ObjectPool(ObjectPool&&) = delete;
		ObjectPool& operator=(const ObjectPool&) = delete;
		ObjectPool& operator=(ObjectPool&&) = delete;

		static_assert(std::is_base_of_v<Handleable<T>, T>, "Object must be Handleable");
	};
}

#include "ObjectPool.inl"