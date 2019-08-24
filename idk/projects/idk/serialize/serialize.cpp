#include "stdafx.h"
#include "serialize.h"
#include <reflect/reflect.h>
#include <res/Guid.h>
#include <core/GameObject.h>
#include <scene/Scene.h>
#include <common/Transform.h>

#include <json/json.hpp>
using json = nlohmann::basic_json<std::map, std::vector, idk::string>;

namespace idk
{
	static json serialize_json(const reflect::dynamic& obj); // forward decl

	static json serialize_json(const reflect::uni_container& container)
	{
		json j;
		for (auto elem : container)
		{
			if (elem.type.is_template<std::pair>()) // maps
			{
				auto pair = elem.unpack();
				j.emplace(serialize_text(pair[0]), serialize_json(pair[1]));
			}
			else
			{
				j.push_back(serialize_json(elem));
			}
		}
		return j;
	}

	static json serialize_json(const reflect::dynamic& obj)
	{
		if (obj.type.count() == 0)
		{
#define SERIALIZE_CASE(TYPE) case reflect::typehash<TYPE>() : return json(obj.get<TYPE>());
			switch (obj.type.hash())
			{
				SERIALIZE_CASE(int);
				SERIALIZE_CASE(bool);
				SERIALIZE_CASE(char);
				SERIALIZE_CASE(int64_t);
				SERIALIZE_CASE(uint64_t);
				SERIALIZE_CASE(float);
				SERIALIZE_CASE(double);
				case reflect::typehash<string>(): return json(serialize_text(obj.get<string>()));
				case reflect::typehash<Guid>(): return json(serialize_text(obj.get<Guid>()));
			default: break;
			}
#undef SERIALIZE_CASE

			if (obj.type.is_container())
			{
				auto cont = obj.to_container();
				return serialize_json(cont);
			}
			else if (obj.type.is_enum_type())
				return json(obj.to_enum_value().name());
			else
				throw "unhandled case?";
		}

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
			else if (obj.type.is_container())
				return serialize_json(obj.to_container()).dump(2);
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

	static void parse_json(const json& j, reflect::dynamic& obj); // forward decl

	static void parse_json(const json& j, reflect::uni_container& container)
	{
		if (container.type.is_template<std::array>())
		{
			auto container_iter = container.begin();
			auto sz = container.size();
			size_t i = 0;
			for (auto iter = j.begin(); iter != j.end() && i < sz; ++iter, ++i)
			{
				auto elem = *container_iter;
				parse_json(iter.value(), elem);
				++container_iter;
			}
		}
		else
		{
			container.clear();

			if (j.is_array())
			{
				for (auto& elem : j)
				{
					auto dyn = container.value_type.create();
					parse_json(elem, dyn);
					container.add(dyn);
				}
			}
			else if(j.is_object())
			{
				if (container.value_type.is_template<std::pair>())
				{
					for (auto& elem : j.items())
					{
						auto pair = container.value_type.create();
						auto unpacked = pair.unpack();
						parse_text(elem.key(), unpacked[0]);
						parse_json(elem.value(), unpacked[1]);
						container.add(pair);
					}
				}
				else
					throw "set???";
			}
		}

	}

	static void parse_json(const json& j, reflect::dynamic& obj)
	{
		if (obj.type.count() == 0)
		{
#define SERIALIZE_CASE(TYPE) case reflect::typehash<TYPE>() : obj = j.get<TYPE>();
			switch (obj.type.hash())
			{
				SERIALIZE_CASE(int);
				SERIALIZE_CASE(bool);
				SERIALIZE_CASE(char);
				SERIALIZE_CASE(int64_t);
				SERIALIZE_CASE(uint64_t);
				SERIALIZE_CASE(float);
				SERIALIZE_CASE(double);
#undef SERIALIZE_CASE
#define SERIALIZE_CASE(TYPE) case reflect::typehash<TYPE>() : parse_text(j.get<string>(), obj); break;
				SERIALIZE_CASE(string);
				SERIALIZE_CASE(Guid);
				default: break;
			}
#undef SERIALIZE_CASE

			if (obj.type.is_container())
			{
				auto cont = obj.to_container();
				parse_json(j, cont);
			}
			else if (obj.type.is_enum_type())
				obj.to_enum_value().try_assign(j.get<string>());
			else
				throw "unhandled case?";
		}

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