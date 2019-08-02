#pragma once

namespace idk
{
	template<typename T, typename U>
	T s_cast(U&& rhs);

	template<typename T, typename U>
	T d_cast(U&& rhs);

	template<typename T, typename U>
	T r_cast(U&& rhs);

	template<typename T, typename U>
	T c_cast(U&& rhs);
}

#include "casts.inl"