#pragma once
#include <utility>

namespace idk
{
	template<typename T, template<typename...> typename Tag>
	struct has_tag 
	{
	private:
		template<typename ...U> static std::true_type  assign(const Tag<U...>&);
		template<typename ...U> static std::false_type assign(const U&...);
	public:
		static constexpr auto value = std::decay_t<decltype(assign(std::declval<T>))>::value;
	};
}