#include "stdafx.h"
#include "serialize.h"
#include <reflect/reflect.h>
#include <res/Guid.h>
#include <core/GameObject.h>
#include <scene/Scene.h>
#include <common/Transform.h>

#include <json/json.hpp>
using json = nlohmann::json;

namespace idk
{
	static json serialize_json(const reflect::dynamic& obj)
	{
		json j;
		vector<json*> stack{ &j };

		obj.visit([&](auto&& key, auto&& arg, int depth_change)
		{
			using K = std::decay_t<decltype(key)>;
			using T = std::decay_t<decltype(arg)>;
			if (depth_change == -1)
				stack.pop_back();

			if constexpr (std::is_arithmetic_v<K>)
			{
				if constexpr (std::is_arithmetic_v<T>)
					(*stack.back())[key] = arg;
				else if constexpr (is_basic_serializable_v<T>)
					(*stack.back())[key] = serialize_text(arg);
				else if constexpr (is_sequential_container_v<T>)
					stack.push_back(&((*stack.back())[key] = json::array()));
				else // associative container
					stack.push_back(&((*stack.back())[key] = json::object()));
			}
			else if constexpr (is_basic_serializable_v<K>)
			{
				if constexpr (std::is_arithmetic_v<T>)
					(*stack.back())[serialize_text(key)] = arg;
				else if constexpr (is_basic_serializable_v<T>)
					(*stack.back())[serialize_text(key)] = serialize_text(arg);
				else if constexpr (is_sequential_container_v<T>)
					stack.push_back(&((*stack.back())[serialize_text(key)] = json::array()));
				else // associative container
					stack.push_back(&((*stack.back())[serialize_text(key)] = json::object()));
			}
			else
			{
				throw "wtf is this key??";
			}
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
			default: break;
			}
#undef SERIALIZE_CASE

			if (obj.type.is_enum_type())
				return serialize_text(obj.to_enum_value().name());
			else
				throw "unhandled case?";
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



	template <typename T, typename = void>
	struct has_resize : std::false_type {};
	template <typename T>
	struct has_resize<T, std::void_t<decltype(std::declval<T>().clear())>> : std::true_type {};

	void parse_json(const json& j, reflect::dynamic& obj)
	{
		vector<const json*> stack{ &j };

		obj.visit([&](auto&& key, auto&& arg, int depth_change)
		{
			using K = std::decay_t<decltype(key)>;
			using T = std::decay_t<decltype(arg)>;
			if (depth_change == -1)
				stack.pop_back();

			if constexpr (std::is_same_v<K, const char*>)
			{
				auto& curr_j = *stack.back();
				auto iter = curr_j.find(key);
				if (iter == curr_j.end())
					return false;

				if constexpr (std::is_arithmetic_v<T>)
				{
					arg = iter->get<T>();
					return false;
				}
				else if constexpr (is_basic_serializable_v<T>)
				{
					parse_text(iter->get<string>(), arg);
					return false;
				}
				else if constexpr (is_sequential_container_v<T>)
				{
					if constexpr (has_resize<T>::value)
					{
						arg.clear();
						arg.resize(iter->size());
					}
					else
						assert(arg.size() >= iter->size());

					int i = 0;
					for (auto& elem : *iter)
					{
						if constexpr (is_basic_serializable_v<decltype(*arg.begin())>)
							parse_text(elem.get<string>(), arg[i]);
						else
						{
							reflect::dynamic d{ arg[i] };
							parse_json(elem, d);
						}
						++i;
					}
					return false;
				}
				else if constexpr (is_associative_container_v<T>)
				{
					arg.clear();
					for (auto &[elem_name, elem_val] : iter->items())
					{
						if constexpr (is_template_v<std::decay_t<decltype(*arg.begin())>, std::pair>) // map / unordered_map
						{
							using ElemK = std::decay_t<decltype(arg.begin()->first)>;
							using ElemV = std::decay_t<decltype(arg.begin()->second)>;
							if constexpr(is_basic_serializable_v<ElemV>)
								arg.emplace(parse_text<ElemK>(elem_name), parse_text<ElemV>(elem_val.get<string>()));
							else
								parse_json(elem_val, arg.emplace(parse_text<ElemK>(elem_name), ElemV{}).second);
						}
						else
						{
							using ElemV = std::decay_t<decltype(*arg.begin())>;
							if constexpr (is_basic_serializable_v<decltype(arg.begin()->second)>)
								arg.emplace(parse_text<ElemV>(elem_val.get<string>()));
							else
							{
								ElemV elem_to_emplace{};
								parse_json(elem_val, elem_to_emplace);
								arg.emplace(std::move(elem_to_emplace));
							}
						}
					}
					return false;
				}
				else // not basic serializable and not container; go deeper!
				{
					stack.push_back(&*iter);
					return true;
				}
			}
			else
			{
				throw "how did we get here?";
			}
		});
	}

	reflect::dynamic parse_text(const string& str, reflect::type type)
	{
		const json j = json::parse(str);
		auto obj = type.create();
		parse_json(j, obj);
		return obj;
	}

	template<>
	void parse_text(const string& str, Scene& scene)
	{
		const json j = json::parse(str);
		for (auto elem : j)
		{
			Handle<GameObject> handle{ elem["id"].get<uint64_t>() };
			scene.CreateGameObject(handle);

			for (auto& component_j : elem.items())
			{
				if (component_j.key() == "id")
					continue;

				auto type = reflect::get_type(component_j.key());
				if (type.is<Transform>())
				{
					reflect::dynamic obj{ *handle->GetComponent<Transform>() };
					parse_json(component_j.value(), obj);
				}
				else
				{
					reflect::dynamic obj{ *handle->AddComponent(type) };
					parse_json(component_j.value(), obj);
				}
			}
		}
	}
}