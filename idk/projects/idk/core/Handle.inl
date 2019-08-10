
#pragma once
#include "Handle.h"
#include "GameState.h"

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
	Handle<T>::Handle(uint32_t index, uint16_t gen, uint8_t scene)
		: GenericHandle{ type_id, scene, gen, index }
	{
	}

	template<typename T>
	Handle<T>::operator bool() const
	{
		return GameState::GetGameState().ValidateHandle(*this);
	}

	template<typename T>
	T& Handle<T>::operator*() const
	{
		return *this->operator->();
	}

	template<typename T>
	T* Handle<T>::operator->() const
	{
		return GameState::GetGameState().GetObject(*this);
	}

	template <typename T>
	Handle<T> handle_cast(const GenericHandle& handle)
	{
		return Handle<T>{handle};
	}
}