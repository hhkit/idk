#pragma once

#include <idk.h>

namespace idk
{

	// true if is integral/floating point, or is convertible to string
	template<typename T, typename>
	struct is_basic_serializable;

	// true if is integral/floating point, or is convertible to string
	template<typename T>
	constexpr auto is_basic_serializable_v = is_basic_serializable<T>::value;


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
	void parse_text(const string& str, T& obj);

	template<typename T>
	T parse_text(const string& str);

	template<> // parse scene
	void parse_text(const string& str, Scene& scene);

	reflect::dynamic parse_text(const string& str, reflect::type type);

}

#include "serialize.inl"