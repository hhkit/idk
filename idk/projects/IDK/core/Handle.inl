#pragma once

namespace idk
{


	template<typename T>
	bool GenericHandle::is_type() const
	{
		return Handle<T>::type_id == type;
	}

	template<typename T>
	inline Handle<T>::Handle(const GenericHandle& handle)
		: GenericHandle{handle.id}
	{
		assert(handle.is_type<T>());
	}

	template<typename T>
	Handle<T>::Handle(uint32_t index, uint16_t uses, uint8_t scene)
		: GenericHandle{ type_id, scene, uses, index }
	{
	}

	template <typename T>
	Handle<T> handle_cast(const GenericHandle& handle)
	{
		return Handle<T>{handle};
	}
}