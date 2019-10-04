#include "stdafx.h"
#include "serialize.h"

#include <reflect/reflect.h>
#include <serialize/yaml.h>

#include <core/GameObject.h>
#include <scene/Scene.h>
#include <common/Transform.h>
#include <common/Name.h>

namespace idk
{
	static yaml::node serialize_yaml(const reflect::dynamic& obj); // forward decl

	static yaml::node serialize_yaml(const reflect::uni_container& container)
	{
		yaml::node node;

		if (container.value_type.is_template<std::pair>()) // associative
		{
            if (container.size() == 0)
                return yaml::node{ yaml::mapping_type() };

			for (auto elem : container)
			{
				auto pair = elem.unpack();
                if (pair[1].type.hash() == reflect::typehash<reflect::dynamic>()) // stores a dynamic?!
                {
                    auto node_right = serialize_yaml(pair[1]);
                    if (pair[1].valid())
                        node_right.tag(pair[1].type.name());
                    node.emplace(serialize_text(pair[0]), node_right);
                }
				else
                    node.emplace(serialize_text(pair[0]), serialize_yaml(pair[1]));
			}
		}
		else // sequential
		{
            if (container.size() == 0)
                return yaml::node{ yaml::sequence_type() };

			if (container.value_type.hash() == reflect::typehash<reflect::dynamic>()) // stores a dynamic?!
			{
                for (auto elem : container)
                {
                    auto node_item = serialize_yaml(elem);
                    if (elem.valid())
                        node_item.tag(elem.type.name());
                    node.push_back(node_item);
                }
			}
			else
			{
				for (auto elem : container)
                    node.push_back(serialize_yaml(elem));
			}
		}

		return node;
	}

	static yaml::node serialize_yaml(const reflect::dynamic& obj)
	{
        if (!obj.valid())
            return yaml::node();
        else if (obj.type.is_enum_type())
            return yaml::node(obj.to_enum_value().name());
		else if (obj.type.count() == 0)
		{
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
                yaml::node ret = serialize_yaml(held);
                ret.tag(held.type.name());
                return ret;
			}
            else if (obj.type.hash() == reflect::typehash<reflect::dynamic>())
            {
                auto& held = obj.get<reflect::dynamic>();
                yaml::node ret = serialize_yaml(held);
                ret.tag(held.type.name());
                return ret;
            }
			else
				throw "unhandled case?";
		}
        else if (obj.type.is_basic_serializable())
            return yaml::node(obj.to_string());

        yaml::node node;
		vector<yaml::node*> stack{ &node };

		constexpr static auto f = [](auto&& k, auto&& arg, vector<yaml::node*>& stack)
		{
			using K = std::decay_t<decltype(k)>;
			using T = std::decay_t<decltype(arg)>;

            auto& curr_node = *stack.back();
			if constexpr (std::is_arithmetic_v<T>)
			{
                if constexpr (std::is_arithmetic_v<K>)
                    curr_node.emplace_back(arg);
                else
                    curr_node[k] = arg;
				return true;
			}
			else if constexpr (is_basic_serializable_v<T>)
			{
                if constexpr (std::is_arithmetic_v<K>)
                    curr_node.emplace_back(serialize_text(arg));
                else
                    curr_node.emplace(k, serialize_text(arg));
				return true;
			}
			else if constexpr (is_associative_container_v<T>)
			{
                if constexpr (std::is_arithmetic_v<K>)
                    stack.push_back(&curr_node.emplace_back(yaml::mapping_type()));
                else
                    stack.push_back(&(curr_node[k] = yaml::mapping_type()));

                if (arg.size() == 0)
                {
                    stack.pop_back();
                    return false;
                }
				return true;
			}
            else if constexpr (is_sequential_container_v<T>)
            {
                if constexpr (std::is_arithmetic_v<K>)
                    stack.push_back(&curr_node.emplace_back(yaml::sequence_type()));
                else
                    stack.push_back(&(curr_node[k] = yaml::sequence_type()));

                if (arg.size() == 0)
                {
                    stack.pop_back();
                    return false;
                }
                return true;
            }
            else if constexpr (is_template_v<T, std::variant>)
            {
                yaml::node var_node = serialize_yaml(arg);
                if constexpr (std::is_arithmetic_v<K>)
                    curr_node.emplace_back(var_node);
                else
                    curr_node.emplace(k, var_node);
                return false;
            }
			else if constexpr (std::is_same_v<T, reflect::dynamic>)
			{
                yaml::node dyn_node = serialize_yaml(arg);
                if (arg.valid() && !dyn_node.has_tag())
                    dyn_node.tag(arg.type.name());
                if constexpr (std::is_arithmetic_v<K>)
                    curr_node.emplace_back(dyn_node);
                else
                    curr_node.emplace(k, dyn_node);
				return false;
			}
			else
			{
                yaml::node node = serialize_yaml(reflect::dynamic{ arg });
                if constexpr (std::is_arithmetic_v<K>)
                    curr_node.emplace_back(node);
                else
                    curr_node[k] = node;
				return false;
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

		return node;
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
            elem.emplace_back(yaml::mapping_type{ { "active", yaml::node{obj.ActiveSelf()} } });
			for (auto& handle : obj.GetComponents())
                elem.emplace_back(serialize_yaml(*handle)).tag((*handle).type.name());
		}

        return yaml::dump(node);
	}


	template <typename T, typename = void>
	struct has_resize : std::false_type {};
	template <typename T>
	struct has_resize<T, std::void_t<decltype(std::declval<T>().clear())>> : std::true_type {};

	static void parse_yaml(const yaml::node& node, reflect::dynamic& obj); // forward decl

	static void parse_yaml(const yaml::node& node, reflect::uni_container& container)
	{
		if (container.type.is_template<std::array>())
		{
			auto container_iter = container.begin();
			const auto sz = container.size();
			size_t i = 0;
			for (auto iter = node.begin(); iter != node.end() && i < sz; ++iter, ++i)
			{
				auto elem = *container_iter;
				parse_yaml(*iter, elem);
				++container_iter;
			}
		}
		else
		{
			container.clear();

			if (node.is_sequence())
			{
				for (auto& elem : node)
				{
					auto dyn = container.value_type.create();
                    parse_yaml(elem, dyn);
					container.add(dyn);
				}
			}
			else if(node.is_mapping())
			{
				if (container.value_type.is_template<std::pair>())
				{
					for (auto& [key, item_node] : node.as_mapping())
					{
						auto pair = container.value_type.create();
						auto unpacked = pair.unpack();
						parse_text(key, unpacked[0]);
                        parse_yaml(item_node, unpacked[1]);
						container.add(pair);
					}
				}
				else
					throw "set???";
			}
		}

	}

	static void parse_yaml(const yaml::node& node, reflect::dynamic& obj)
	{
        if (node.is_null())
            return;
		if (!obj.valid())
		{
            if (node.has_tag())
                new (&obj) reflect::dynamic{ reflect::get_type(node.tag()).create() };
			else
				throw "invalid dynamic passed in!";
		}

        if (obj.type.is_enum_type())
        {
            obj = obj.to_enum_value().try_assign(node.as_scalar()).value();
            return;
        }
		else if (obj.type.count() == 0)
		{
#define SERIALIZE_CASE(TYPE) case reflect::typehash<TYPE>() : obj = node.get<TYPE>(); return;
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
                obj = node.as_scalar();
                return;
            }
			else if (obj.type.is_container())
			{
				auto cont = obj.to_container();
				parse_yaml(node, cont);
                return;
			}
            else if (obj.type.hash() == reflect::typehash<reflect::dynamic>())
            {
                auto& held = obj.get<reflect::dynamic>();
                parse_yaml(node, held);
                return;
            }
			else
				throw "unhandled case?";
		}
        else if (obj.type.is_basic_serializable())
        {
            obj = node.as_scalar();
            return;
        }

		vector<const yaml::node*> stack{ &node };

		obj.visit([&](auto&& key, auto&& arg, int depth_change)
		{
			using K = std::decay_t<decltype(key)>;
			using T = std::decay_t<decltype(arg)>;
			while (++depth_change <= 0)
				stack.pop_back();

			if constexpr (std::is_same_v<K, const char*>)
			{
				auto& curr_node = *stack.back();
				auto iter = curr_node.as_mapping().find(key);
				if (iter == curr_node.as_mapping().end())
					return false;

                auto& [item_key, item_node] = *iter;

				if constexpr (std::is_arithmetic_v<T>)
				{
					arg = item_node.get<T>();
					return false;
				}
				else if constexpr (is_basic_serializable_v<T>)
				{
					parse_text(item_node.as_scalar(), arg);
					return false;
				}
				else if constexpr (is_sequential_container_v<T>)
				{
					if constexpr (has_resize<T>::value)
					{
						arg.clear();
						arg.resize(item_node.size());
					}
					else // sequence node should have equal or less items than array
						assert(arg.size() >= item_node.size());

					int i = 0;
					for (auto& elem : item_node)
					{
						if constexpr (is_basic_serializable_v<decltype(*arg.begin())>)
							parse_text(elem.as_scalar(), arg[i]);
                        else if constexpr(std::is_same_v<std::decay_t<decltype(*arg.begin())>, reflect::dynamic>)
                        {
                            parse_yaml(elem, arg[i]);
                        }
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
					for (auto &[elem_name, elem_val] : item_node.as_mapping())
					{
						if constexpr (is_template_v<std::decay_t<decltype(*arg.begin())>, std::pair>) // map / unordered_map
						{
							using ElemK = std::decay_t<decltype(arg.begin()->first)>;
							using ElemV = std::decay_t<decltype(arg.begin()->second)>;
							if constexpr(is_basic_serializable_v<ElemV>)
								arg.emplace(parse_text<ElemK>(elem_name), parse_text<ElemV>(elem_val.get<string>()));
                            else
                            {
                                reflect::dynamic d = arg.emplace(parse_text<ElemK>(elem_name), ElemV{}).first->second;
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
					auto dyn = reflect::get_type(item_node.tag()).create();
					parse_yaml(item_node, dyn);
					reflect::dynamic{ arg } = dyn;
					return false;
				}
                else if constexpr (std::is_same_v<T, reflect::dynamic>) // arg not reflected in ReflectedTypes
                {
                    parse_yaml(item_node, arg);
                    return false;
                }
				else // not basic serializable and not container and not variant; go deeper!
				{
					stack.push_back(&item_node);
					return true;
				}
			}

			else
			{
				throw "how did we get here?";
			}
		}); // visit

        obj.on_parse();
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
			const Handle<GameObject> handle{ parse_text<uint64_t>(elem.tag()) };
			scene.CreateGameObject(handle);

            auto iter = elem.begin();
            handle->SetActive(iter->at("active").get<bool>());

			for (++iter; iter != elem.end(); ++iter)
			{
				const auto type = reflect::get_type(iter->tag());
				if (type.is<Transform>())
				{
					reflect::dynamic obj{ *handle->GetComponent<Transform>() };
					parse_yaml(*iter, obj);
				}
                else if (type.is<Name>())
                {
                    handle->GetComponent<Name>()->name = iter->at("name").as_scalar();
                }
				else
				{
					reflect::dynamic obj{ *handle->AddComponent(type) };
                    parse_yaml(*iter, obj);
				}
			}
		}
	}
}