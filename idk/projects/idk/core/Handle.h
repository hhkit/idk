#pragma once
#include <tuple>
#include <string>

#include <idk.h>
#include <meta/meta.h>
#include "Handleables.h"

namespace idk::reflect
{
	class dynamic;
}


#pragma warning(disable:4201)

namespace idk
{
	struct GenericHandle
	{
		using type_t  = uint8_t;
		using scene_t = uint8_t;
		using gen_t  = uint16_t;
		using index_t = uint32_t;

		union {
			struct {
				type_t  type;
				scene_t scene;
				gen_t   gen;
				index_t index;
			};

			uint64_t id = 0;
		};

		GenericHandle() = default;
		explicit GenericHandle(uint64_t id);
		GenericHandle(uint8_t  type, uint8_t  scene, uint16_t gen, uint32_t index);

		template<typename T> 
		bool is_type() const;

		reflect::dynamic operator*() const;
		operator bool() const;
		bool operator==(const GenericHandle&);
		bool operator!=(const GenericHandle&);
	};

	template<typename T>
	class Handle 
		: public GenericHandle
	{
	public:
		static constexpr auto type_id = index_in_tuple_v<T, idk::Handleables>;

		Handle() = default;
		explicit Handle(uint64_t id);
		explicit Handle(const GenericHandle&);
		Handle(uint32_t index, uint16_t gen, uint8_t scene = 0);

		explicit operator bool() const;
		T& operator*() const;
		T* operator->() const;
	};

	template <typename T>
	Handle<T> handle_cast(const GenericHandle&);

	template <typename T>
	class ObjectPool;

	template<typename T>
	class Handleable
	{
	public:
		const Handle<T>& GetHandle() { return handle; }
	protected:
		Handleable() = default;
		Handleable(const Handleable&) : Handleable{} {};
		Handleable& operator=(const Handleable&) { return *this; }
	private:
		Handle<T> handle;
		friend class ObjectPool<T>;
	};
}

#include "Handle.inl"