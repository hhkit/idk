#pragma once

namespace idk
{
	template<typename T, template<typename...> typename Tag>
	struct has_tag;

	template<typename T, template<typename...> typename Tag>
	constexpr static auto has_tag_v = has_tag<T, Tag>::value;
}
#include "tag.inl"