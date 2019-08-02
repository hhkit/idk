#include "ObjectHandle.h"
#pragma once

namespace idk
{
	template<typename T>
	inline ObjectHandle<T>::ObjectHandle(const GenericHandle& handle)
		: GenericHandle{handle.id}
	{
		assert(ObjectHandle<T>::type_id == handle.type);
	}

	template<typename T>
	ObjectHandle<T>::ObjectHandle(uint32_t index, uint16_t uses, uint8_t scene)
		: GenericHandle{ type_id, scene, uses, index }
	{
	}

	template <typename T>
	ObjectHandle<T> handle_cast(const GenericHandle& handle)
	{
		return ObjectHandle<T>{handle};
	}
}