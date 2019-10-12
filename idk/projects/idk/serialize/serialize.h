#pragma once

#include <idk.h>

namespace idk
{

    enum class serialize_error : char
    {
        invalid_argument,
        result_out_of_range
    };



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
    T parse_text(string_view sv);

	template<typename T>
	void parse_text(string_view sv, T& obj);

    template<>
    void parse_text(string_view sv, reflect::dynamic& obj);

	template<> // parse scene
	void parse_text(string_view sv, Scene& scene);

	reflect::dynamic parse_text(string_view sv, reflect::type type);

}

#include "serialize.inl"