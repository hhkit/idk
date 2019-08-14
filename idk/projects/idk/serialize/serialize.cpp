#include "stdafx.h"
#include "serialize.h"
#include <reflect/reflect.h>
#include <res/Guid.h>
#include <core/GameObject.h>
#include <core/Scene.h>
#include <common/Transform.h>

#include <json/json.hpp>
using json = nlohmann::json;

namespace idk
{
	static json serialize_json(const reflect::dynamic& obj)
	{
		json j;
		vector<json*> stack{ &j };

		obj.visit([&](const char* name, auto&& arg, int depth_change)
		{
			using T = std::decay_t<decltype(arg)>;
			if (depth_change == -1)
				stack.pop_back();

			if constexpr (std::is_arithmetic_v<T>)
				(*stack.back())[name] = arg;
			else if constexpr (is_basic_serializable<T>::value)
				(*stack.back())[name] = serialize_text(arg);
			else
				stack.push_back(&((*stack.back())[name] = json::object()));
		});

		return j;
	}

	template<>
	string serialize_text(const reflect::dynamic& obj)
	{
		if (obj.type.count() == 0)
		{
#define SERIALIZE_CASE(TYPE) case reflect::typehash<TYPE>() : return serialize_text(obj.get<TYPE>())
			switch (obj.type.hash())
			{
			SERIALIZE_CASE(int);
			SERIALIZE_CASE(bool);
			SERIALIZE_CASE(char);
			SERIALIZE_CASE(int64_t);
			SERIALIZE_CASE(uint64_t);
			SERIALIZE_CASE(float);
			SERIALIZE_CASE(double);
			SERIALIZE_CASE(std::string);
			SERIALIZE_CASE(Guid);
			default: throw "Unhandled case?";
			}
#undef SERIALIZE_CASE
		}

		return serialize_json(obj).dump(2);
	}

	template<>
	string serialize_text(const Scene& scene)
	{
		json j = json::array();
		for (auto& obj : scene)
		{
			json& elem = j.emplace_back(json::object());
			elem["id"] = obj.GetHandle().id;
			for (auto& handle : obj.GetComponents())
			{
				elem[string{ (*handle).type.name() }] = serialize_json(*handle);
			}
		}

		return j.dump(2);
	}



	reflect::dynamic parse_json(const json& j, reflect::type type)
	{
		vector<const json*> stack{ &j };

		auto obj = type.create();

		obj.visit([&](const char* name, auto&& arg, int depth_change)
		{
			using T = std::decay_t<decltype(arg)>;
			if (depth_change == -1)
				stack.pop_back();

			auto& curr_j = *stack.back();
			auto iter = curr_j.find(name);
			if (iter == curr_j.end())
				return false;

			if constexpr (!is_basic_serializable<T>::value)
			{
				stack.push_back(&*iter);
				return true;
			}
			else if constexpr (std::is_arithmetic_v<T>)
			{
				arg = iter->get<T>();
				return false;
			}
			else
			{
				arg = parse_text<T>(iter->get<string>());
				return false;
			}
		});

		return obj;
	}

	reflect::dynamic parse_text(const string& str, reflect::type type)
	{
		const json j = json::parse(str);
		return parse_json(j, type);
	}

	template<>
	void parse_text(const string& str, Scene& scene)
	{
		const json j = json::parse(str);
		for (auto elem : j)
		{
			Handle<GameObject> handle{ elem["id"].get<uint64_t>() };
			scene.CreateGameObject(handle);

			for (auto& component : elem.items())
			{
				if (component.key() == "id")
					continue;

				auto type = reflect::get_type(component.key());
				if(type.hash() == reflect::typehash<Transform>())
					reflect::dynamic{ *handle->GetComponent<Transform>() } = parse_json(component.value(), type);
				else
					*handle->AddComponent(type) = parse_json(component.value(), type);
			}
		}
	}
}