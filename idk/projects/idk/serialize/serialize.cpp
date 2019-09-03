#include "stdafx.h"
#include "serialize.h"

#include <reflect/reflect.h>
#include <serialize/yaml.h>

#include <core/GameObject.h>
#include <scene/Scene.h>
#include <common/Transform.h>

namespace idk
{
	static yaml::node serialize_yaml(const reflect::dynamic& obj); // forward decl

	static yaml::node serialize_yaml(const reflect::uni_container& container)
	{
		yaml::node j;

		if (container.value_type.is_template<std::pair>()) // associative
		{
			for (auto elem : container)
			{
				auto pair = elem.unpack();
                if (pair[1].type.hash() == reflect::typehash<reflect::dynamic>()) // stores a dynamic?!
                {
                    auto node_right = serialize_yaml(pair[1]);
                    if (pair[1].valid())
                        node_right.tag(pair[1].type.name());
                    j.emplace(serialize_text(pair[0]), node_right);
                }
				else
					j.emplace(serialize_text(pair[0]), serialize_yaml(pair[1]));
			}
		}
		else // sequential
		{
			if (container.value_type.hash() == reflect::typehash<reflect::dynamic>()) // stores a dynamic?!
			{
                for (auto elem : container)
                {
                    auto node_item = serialize_yaml(elem);
                    if (elem.valid())
                        node_item.tag(elem.type.name());
                    j.push_back(node_item);
                }
			}
			else
			{
				for (auto elem : container)
					j.push_back(serialize_yaml(elem));
			}
		}

		return j;
	}

	static yaml::node serialize_yaml(const reflect::dynamic& obj)
	{
        if (!obj.valid())
            return yaml::node();
        else if (obj.type.is_enum_type())
            return yaml::node(obj.to_enum_value().name());
		else if (obj.type.count() == 0)
		{
#define SERIALIZE_CASE(TYPE) case reflect::typehash<TYPE>() : return yaml::node(obj.get<TYPE>());
            switch (obj.type.hash())
            {
                SERIALIZE_CASE(int);
                SERIALIZE_CASE(bool);
                SERIALIZE_CASE(char);
                SERIALIZE_CASE(int64_t);
                SERIALIZE_CASE(uint64_t);
                SERIALIZE_CASE(float);
                SERIALIZE_CASE(double);
            }
#undef SERIALIZE_CASE

            if (obj.type.is_basic_serializable())
                return yaml::node(obj.to_string());
			else if (obj.type.is_container())
			{
				auto cont = obj.to_container();
				return serialize_yaml(cont);
			}
			else if (obj.type.is_template<std::variant>())
			{
				auto held = obj.get_variant_value();
                yaml::node ret;
                ret.emplace(held.type.name(), serialize_yaml(held));
			}
			else
				throw "unhandled case?";
		}
        else if (obj.type.is_basic_serializable())
            return yaml::node(obj.to_string());

        yaml::node j;
		vector<yaml::node*> stack{ &j };

		constexpr static auto f = [](auto&& k, auto&& arg, vector<yaml::node*>& stack)
		{
			using K = std::decay_t<decltype(k)>;
			using T = std::decay_t<decltype(arg)>;

			if constexpr (std::is_arithmetic_v<T>)
			{
				(*stack.back())[k] = arg;
				return true;
			}
			else if constexpr (is_basic_serializable_v<T>)
			{
                if constexpr (std::is_arithmetic_v<K>)
                    stack.back()->emplace_back(serialize_text(arg));
                else
                    stack.back()->emplace(k, serialize_text(arg));
				return true;
			}
			else if constexpr (is_sequential_container_v<T>)
			{
				stack.push_back(&(*stack.back())[k]);
				return true;
			}
			else if constexpr (is_associative_container_v<T>)
			{
                stack.push_back(&(*stack.back())[k]);
				return true;
			}
			else if constexpr (std::is_same_v<T, reflect::dynamic>)
			{
                yaml::node dyn_node = serialize_yaml(arg);
                if (arg.valid())
                    dyn_node.tag(arg.type.name());
                (*stack.back())[k] = dyn_node;
				return false;
			}
			else
			{
				stack.push_back(&((*stack.back())[k]));
				return true;
			}
		};

		obj.visit([&](auto&& key, auto&& arg, int depth_change)
		{
			using K = std::decay_t<decltype(key)>;
			while (++depth_change <= 0)
				stack.pop_back();

			if constexpr (std::is_arithmetic_v<K>)
				return f(key, arg, stack);
			else if constexpr (is_basic_serializable_v<K>)
				return f(serialize_text(key), arg, stack);
			else if constexpr (std::is_same_v<K, reflect::type>) // variant element
				return f(string{ key.name() }, arg, stack);
			else
				throw "wtf is this key??";
		});

		return j;
	}

	template<>
	string serialize_text(const reflect::dynamic& obj)
	{
        return yaml::dump(serialize_yaml(obj));
	}

	template<>
	string serialize_text(const Scene& scene)
	{
        yaml::node node;
		for (auto& obj : scene)
		{
            yaml::node& elem = node.emplace_back();
            elem.tag(serialize_text(obj.GetHandle().id));
			for (auto& handle : obj.GetComponents())
                elem.emplace_back(serialize_yaml(*handle)).tag((*handle).type.name());
		}

        return yaml::dump(node);
	}


	template <typename T, typename = void>
	struct has_resize : std::false_type {};
	template <typename T>
	struct has_resize<T, std::void_t<decltype(std::declval<T>().clear())>> : std::true_type {};

	static void parse_yaml(const yaml::node& j, reflect::dynamic& obj); // forward decl

	static void parse_yaml(const yaml::node& j, reflect::uni_container& container)
	{
		if (container.type.is_template<std::array>())
		{
			auto container_iter = container.begin();
			auto sz = container.size();
			size_t i = 0;
			for (auto iter = j.begin(); iter != j.end() && i < sz; ++iter, ++i)
			{
				auto elem = *container_iter;
				parse_yaml(*iter, elem);
				++container_iter;
			}
		}
		else
		{
			container.clear();

			if (j.is_sequence())
			{
				for (auto& elem : j)
				{
					auto dyn = container.value_type.create();
                    parse_yaml(elem, dyn);
					container.add(dyn);
				}
			}
			else if(j.is_mapping())
			{
				if (container.value_type.is_template<std::pair>())
				{
					for (auto& elem : j.as_mapping())
					{
						auto pair = container.value_type.create();
						auto unpacked = pair.unpack();
						parse_text(elem.first, unpacked[0]);
                        parse_yaml(elem.second, unpacked[1]);
						container.add(pair);
					}
				}
				else
					throw "set???";
			}
		}

	}

	static void parse_yaml(const yaml::node& j, reflect::dynamic& obj)
	{
        if (j.is_null())
            return;
		if (!obj.valid())
		{
            if (j.has_tag())
                new (&obj) reflect::dynamic{ reflect::get_type(j.tag()).create() };
			else
				throw "invalid dynamic passed in!";
		}

        if (obj.type.is_enum_type())
        {
            obj = obj.to_enum_value().try_assign(j.get<string>()).value();
            return;
        }
		else if (obj.type.count() == 0)
		{
#define SERIALIZE_CASE(TYPE) case reflect::typehash<TYPE>() : obj = j.get<TYPE>(); return;
			switch (obj.type.hash())
			{
				SERIALIZE_CASE(int);
				SERIALIZE_CASE(bool);
				SERIALIZE_CASE(char);
				SERIALIZE_CASE(int64_t);
				SERIALIZE_CASE(uint64_t);
				SERIALIZE_CASE(float);
				SERIALIZE_CASE(double);
				default: break;
			}
#undef SERIALIZE_CASE

            if (obj.type.is_basic_serializable())
            {
                obj = j.get<string>();
                return;
            }
			else if (obj.type.is_container())
			{
				auto cont = obj.to_container();
				parse_yaml(j, cont);
                return;
			}
			else
				throw "unhandled case?";
		}
        else if (obj.type.is_basic_serializable())
        {
            obj = j.get<string>();
            return;
        }

		vector<const yaml::node*> stack{ &j };

		obj.visit([&](auto&& key, auto&& arg, int depth_change)
		{
			using K = std::decay_t<decltype(key)>;
			using T = std::decay_t<decltype(arg)>;
			while (++depth_change <= 0)
				stack.pop_back();

			if constexpr (std::is_same_v<K, const char*>)
			{
				auto& curr_j = *stack.back();
				auto iter = curr_j.as_mapping().find(key);
				if (iter == curr_j.as_mapping().end())
					return false;

				if constexpr (std::is_arithmetic_v<T>)
				{
					arg = iter->second.get<T>();
					return false;
				}
				else if constexpr (is_basic_serializable_v<T>)
				{
					parse_text(iter->second.get<string>(), arg);
					return false;
				}
				else if constexpr (is_sequential_container_v<T>)
				{
					if constexpr (has_resize<T>::value)
					{
						arg.clear();
						arg.resize(iter->second.size());
					}
					else
						assert(arg.size() >= iter->second.size());

					int i = 0;
					for (auto& elem : iter->second)
					{
						if constexpr (is_basic_serializable_v<decltype(*arg.begin())>)
							parse_text(elem.get<string>(), arg[i]);
						else
						{
							reflect::dynamic d{ arg[i] };
                            parse_yaml(elem, d);
						}
						++i;
					}
					return false;
				}
				else if constexpr (is_associative_container_v<T>)
				{
					arg.clear();
					for (auto &[elem_name, elem_val] : iter->second.as_mapping())
					{
						if constexpr (is_template_v<std::decay_t<decltype(*arg.begin())>, std::pair>) // map / unordered_map
						{
							using ElemK = std::decay_t<decltype(arg.begin()->first)>;
							using ElemV = std::decay_t<decltype(arg.begin()->second)>;
							if constexpr(is_basic_serializable_v<ElemV>)
								arg.emplace(parse_text<ElemK>(elem_name), parse_text<ElemV>(elem_val.get<string>()));
                            else
                            {
                                reflect::dynamic d = arg.emplace(parse_text<ElemK>(elem_name), ElemV{}).second;
                                parse_yaml(elem_val, d);
                            }
						}
						else
						{
							using ElemV = std::decay_t<decltype(*arg.begin())>;
							if constexpr (is_basic_serializable_v<decltype(arg.begin()->second)>)
								arg.emplace(parse_text<ElemV>(elem_val.get<string>()));
							else
							{
								ElemV elem_to_emplace{};
                                parse_yaml(elem_val, elem_to_emplace);
								arg.emplace(std::move(elem_to_emplace));
							}
						}
					}
					return false;
				}
				else if constexpr (is_template_v<T, std::variant>)
				{
                    auto alt_type_name = iter->second.as_mapping().begin()->first;
					auto type = reflect::get_type(alt_type_name);
					auto dyn = type.create();
					parse_yaml(iter->second.as_mapping().begin()->second, dyn);
					reflect::dynamic{ arg } = dyn;
					return false;
				}
                else if constexpr (std::is_same_v<T, reflect::dynamic>) // arg not reflected in ReflectedTypes
                {
                    parse_yaml(iter->second, arg);
                    return false;
                }
				else // not basic serializable and not container and not variant; go deeper!
				{
					stack.push_back(&iter->second);
					return true;
				}
			}

			else
			{
				throw "how did we get here?";
			}
		});
	}

    template<>
    void parse_text(const string& str, reflect::dynamic& obj)
    {
        parse_yaml(yaml::load(str), obj);
    }

    reflect::dynamic parse_text(const string& str, reflect::type type)
    {
        auto obj = type.create();
        parse_text(str, obj);
        return obj;
    }

	template<>
	void parse_text(const string& str, Scene& scene)
	{
		const auto node = yaml::load(str);
		for (auto& elem : node)
		{
			Handle<GameObject> handle{ parse_text<uint64_t>(elem.tag()) };
			scene.CreateGameObject(handle);

			for (auto& comp_node : elem)
			{
				auto type = reflect::get_type(comp_node.tag());
				if (type.is<Transform>())
				{
					reflect::dynamic obj{ *handle->GetComponent<Transform>() };
					parse_yaml(comp_node, obj);
				}
				else
				{
					reflect::dynamic obj{ *handle->AddComponent(type) };
                    parse_yaml(comp_node, obj);
				}
			}
		}
	}
}