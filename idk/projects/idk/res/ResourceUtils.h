#pragma once

namespace idk
{
	template<typename Res>
	struct BaseResource;

	template<typename T>
	constexpr auto BaseResourceID = BaseResource<T>::value;
}

