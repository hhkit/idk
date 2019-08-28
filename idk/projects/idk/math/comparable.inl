#pragma once

namespace idk
{
	template<typename T>
	inline bool comparable<T>::operator>(const T& rhs) const
	{
		static_assert(std::is_same_v<decltype(std::declval<const T&>() < std::declval<const T&>()), bool>, "type must be override bool operator<(const T&) const");
		return rhs < me();
	}
	template<typename T>
	inline bool comparable<T>::operator<=(const T& rhs) const
	{
		return !(rhs > me());
	}
	template<typename T>
	inline bool comparable<T>::operator>=(const T& rhs) const
	{
		return !(me() < rhs);
	}
	template<typename T>
	inline bool comparable<T>::operator==(const T& rhs) const
	{
		return !operator!=(rhs);
	}

	template<typename T>
	inline bool comparable<T>::operator!=(const T& rhs) const
	{
		return me() < rhs || rhs < me();
	}

	template<typename T>
	inline T& comparable<T>::me()
	{
		return *static_cast<T*>(this);
	}

	template<typename T>
	inline const T& comparable<T>::me() const
	{
		return *static_cast<const T*>(this);
	}
}