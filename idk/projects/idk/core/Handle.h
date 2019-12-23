#pragma once
#include <tuple>
#include <string>

#include <idk.h>
#include <math/comparable.h>
#include <meta/meta.inl>
#include <meta/tuple.inl>
#include "Handleables.h"

namespace idk::reflect
{
	class dynamic;
}


#pragma warning(disable:4201)

namespace idk
{
	struct GenericHandle
		: comparable<GenericHandle>
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

		GenericHandle() : id{} {};
		explicit GenericHandle(uint64_t id);
		GenericHandle(uint8_t  type, uint8_t  scene, uint16_t gen, uint32_t index);

		template<typename T> 
		bool is_type() const noexcept;

        template<typename Visitor>
        decltype(auto) visit(Visitor&& visitor) const;

		reflect::dynamic operator*() const;
		explicit operator bool() const;
		bool operator<(const GenericHandle&) const;
		bool operator==(const GenericHandle&) const;
		bool operator!=(const GenericHandle&) const;
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
		const Handle<T>& GetHandle() const noexcept { return handle; }
		bool IsQueuedForDestruction() const noexcept { return _queued_for_destruction; }
	protected:
		Handleable() = default;
	private:
		Handle<T> handle;
		bool      _queued_for_destruction = false;

		friend class ObjectPool<T>;
		friend class GameState;
	};
}

namespace std
{
	template<typename T>
	struct hash <idk::Handle<T>>
	{
		size_t operator()(const idk::Handle<T>& id) const noexcept
		{
			return std::hash<size_t>{}(id.id);
		}
	};
}
