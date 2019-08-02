#pragma once
#include <tuple>

#include "../idk.h"
#include "ObjectHandle_detail.h"

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

		using Handleables = std::tuple<
			int
		>;

		GenericHandle() = default;
		explicit GenericHandle(uint64_t id);
		GenericHandle(
			uint8_t  type,
			uint8_t  scene,
			uint16_t uses,
			uint32_t index
		);
	};

	template<typename T>
	class ObjectHandle 
		: public GenericHandle
	{
	public:
		static constexpr auto type_id = detail::index_in_tuple_v<T, GenericHandle::Handleables>;

		ObjectHandle() = default;
		explicit ObjectHandle(const GenericHandle&);
		ObjectHandle(uint32_t index, uint16_t uses, uint8_t scene = 0);

		operator bool() const;
		T& operator*() const;
		T* operator->() const;
	};

	template <typename T>
	ObjectHandle<T> handle_cast(const GenericHandle&);
}

#include "ObjectHandle.inl"