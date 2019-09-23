#pragma once
#include <tuple>
#include <variant>
namespace idk
{
	template<typename Variant, template<class T> typename Template>
	struct variant_wrap;

	template<typename Variant, template<class T> typename Template>
	using variant_wrap_t = typename variant_wrap<Variant, Template>::type;

	template<typename T>
	struct tuple_to_variant;

	template<typename T>
	using tuple_to_variant_t = typename tuple_to_variant<T>::type;
}
#include "variant.inl"