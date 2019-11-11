#pragma once

#include <idk.h>
#include <ds/result.h>
#include <serialize/parse_error.h>

namespace idk
{

	// forward decls
	namespace reflect { class dynamic; class type; }
	class Scene;



	template<typename T>
    string serialize_text(const T& obj);

	template<>
    string serialize_text(const reflect::dynamic& obj);

	template<> // serialize scene
    string serialize_text(const Scene& scene);


    template<typename T>
    monadic::result<T, parse_error> parse_text(string_view sv);

	template<typename T>
    parse_error parse_text(string_view sv, T& obj);

    template<>
    parse_error parse_text(string_view sv, reflect::dynamic& obj);

	template<> // parse scene
    parse_error parse_text(string_view sv, Scene& scene);

    monadic::result<reflect::dynamic, parse_error> parse_text(string_view sv, reflect::type type);

}

#include "text.inl"