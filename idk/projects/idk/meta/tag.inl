#pragma once
#include <utility>
#include "tag.h"

namespace idk
{
	template<typename T, template<typename...> typename Tag>
	struct has_tag 
	{
	private:
		template<typename ...U> 
		static std::true_type  assign(Tag<U...>&);
		static std::false_type assign(...);
	public:
		static constexpr auto value = std::decay_t<decltype(assign(std::declval<T&>()))>::value;
	};
}