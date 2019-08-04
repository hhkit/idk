#pragma once
#include <tuple>
#include <string>

#include <idk.h>
#include "Handleables.h"
#include "Handle_detail.h"

#pragma warning(disable:4201)

namespace idk
{
	struct GenericHandle
	{
		union {
			struct {
				uint8_t  type;
				uint8_t  scene;
				uint16_t uses;
				uint32_t index;
			};

			uint64_t id = 0;
		};

		GenericHandle() = default;
		explicit GenericHandle(uint64_t id);
		GenericHandle(uint8_t  type, uint8_t  scene, uint16_t uses, uint32_t index);

		template<typename T> 
		bool is_type() const;
	};

	template<typename T>
	class Handle 
		: public GenericHandle
	{
	public:
		static constexpr auto type_id = detail::index_in_tuple_v<T, idk::Handleables>;

		Handle() = default;
		explicit Handle(const GenericHandle&);
		Handle(uint32_t index, uint16_t uses, uint8_t scene = 0);

		operator bool() const;
		T& operator*() const;
		T* operator->() const;
	};

	template <typename T>
	class ObjectPool;

	template<typename T>
	class Handleable
	{
	public:
		const Handle<T>& GetHandle() { return handle; }
	private:
		Handle<T> handle;
		friend class ObjectPool<T>;
	};

	template <typename T>
	Handle<T> handle_cast(const GenericHandle&);
}

#include "Handle.inl"