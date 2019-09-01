#pragma once

#include <idk.h>

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
    T parse_text(const string& str);

	template<typename T>
	void parse_text(const string& str, T& obj);

    template<>
    void parse_text(const string& str, reflect::dynamic& obj);

	template<> // parse scene
	void parse_text(const string& str, Scene& scene);

	reflect::dynamic parse_text(const string& str, reflect::type type);

}

#include "serialize.inl"