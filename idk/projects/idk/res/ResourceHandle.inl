#pragma once
#include <type_traits>

#include <res/Resource.h>
#include <res/ResourceManager.h>

namespace idk
{
	template<typename Res>
	inline RscHandle<Res>::operator bool() const
	{
		return Core::GetResourceManager().Validate(*this);
	}
	template<typename Res>
	inline Res& RscHandle<Res>::operator*() const
	{
		return Core::GetResourceManager().Get(*this);
	}
	template<typename Res>
	inline Res* RscHandle<Res>::operator->() const
	{
		return &Core::GetResourceManager().Get(*this);
	}
	template<typename Res>
	inline bool RscHandle<Res>::operator<(const RscHandle& rhs) const
	{
		return guid < rhs.guid;
	}

	template<typename Res>
	inline void RscHandle<Res>::Set(const idk::Resource<Res>& g)
	{
		*this = g.GetHandle();
	}

	template<typename Res>
	template<typename T>
	T& RscHandle<Res>::as() const
	{
		static_assert(std::is_base_of_v<Res, T>, "Can only downcast to inehrited type.");
		return static_cast<T&>(operator*());
	}
	
	template<typename Res>
	template<typename Other, typename>
	inline RscHandle<Res>::operator RscHandle<Other>() const
	{
		return RscHandle<Other>{guid};
	}
}