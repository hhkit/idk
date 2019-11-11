#pragma once

#include "Handle.h"
#include "GameState.h"
#include <core/NullHandleException.h>
#include <meta/variant.h>

namespace idk
{
    namespace detail
    {
        template<typename T>
        struct GenericHandle_visit_helper;

        template<typename ... Ts>
        struct GenericHandle_visit_helper < std::tuple<Ts...> >
        {
            constexpr static auto GenerateVariantLUT()
            {
                using R = variant_wrap_t<tuple_to_variant_t<Handleables>, Handle>;
                using Fn = R(*)(const GenericHandle& h);

                return array<Fn, sizeof...(Ts)>{
                    [](const GenericHandle& h) -> R
                    {
                        return handle_cast<Ts>(h);
                    }...
                };
            }
        };

        struct GenericHandleVisitHelper
        {
            static inline auto lut = GenericHandle_visit_helper<Handleables>::GenerateVariantLUT();
        };
    }

	template<typename T>
	bool GenericHandle::is_type() const noexcept
	{
		return Handle<T>::type_id == type;
	}

    template<typename Visitor>
    decltype(auto) GenericHandle::visit(Visitor&& visitor) const
    {
        using R = decltype(std::visit(visitor, detail::GenericHandleVisitHelper::lut[type](*this)));
        if constexpr (std::is_void_v<R>)
            std::visit(visitor, detail::GenericHandleVisitHelper::lut[type](*this));
        else
            return std::visit(visitor, detail::GenericHandleVisitHelper::lut[type](*this));
    }

	template<typename T>
	inline Handle<T>::Handle(uint64_t id)
		: GenericHandle{id}
	{
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
		auto retval = GameState::GetGameState().GetObject(*this);
		if (!retval)
			throw NullHandleException{*this};
		return retval;
	}

	template <typename T>
	Handle<T> handle_cast(const GenericHandle& handle)
	{
		return Handle<T>{handle};
	}
}