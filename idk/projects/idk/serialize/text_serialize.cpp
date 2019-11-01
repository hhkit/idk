#include "stdafx.h"
#include "text.h"

#include <reflect/reflect.h>
#include <serialize/yaml.h>

#include <core/GameObject.h>
#include <scene/Scene.h>

#include <script/MonoBehavior.h>

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
        
		if (obj.type.is_enum_type())
            return yaml::node(obj.to_enum_value().name());
        
		if (!obj.type.is<mono::ManagedObject>())
		{
			if (obj.type.count() == 0)
			{
				if (obj.type.is_basic_serializable())
					return yaml::node(obj.to_string());

				if (obj.type.is_container())
					return serialize_yaml(obj.to_container());

				if (obj.type.is_template<std::variant>())
				{
					auto held = obj.get_variant_value();
					yaml::node ret = serialize_yaml(held);
					ret.tag(held.type.name());
					return ret;
				}

				if (obj.type.hash() == reflect::typehash<reflect::dynamic>())
				{
					auto& held = obj.get<reflect::dynamic>();
					yaml::node ret = serialize_yaml(held);
					ret.tag(held.type.name());
					return ret;
				}

				throw "unhandled case?";
			}

			if (obj.type.is_basic_serializable())
				return yaml::node(obj.to_string());
		}

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

		auto yaml_visitor = [&](auto&& key, auto&& arg, int depth_change)
		{
			using K = std::decay_t<decltype(key)>;
			while (++depth_change <= 0)
				stack.pop_back();

			if constexpr (std::is_arithmetic_v<K>)
				return f(key, arg, stack);
			else if constexpr (is_basic_serializable_v<K> || std::is_same_v<std::decay_t<K>, const char*>)
				return f(serialize_text(key), arg, stack);
			else if constexpr (std::is_same_v<K, reflect::type>) // variant element
				return f(string{ key.name() }, arg, stack);
			else
				throw "wtf is this key??", arg;
		};

		if (obj.is<mono::ManagedObject>())
		{
			auto& managed_obj = obj.get<mono::ManagedObject>();
			if (!managed_obj.Type())
				return yaml::node();

			managed_obj.Visit(yaml_visitor);
			node.tag(managed_obj.TypeName());
			return node;
		}

		obj.visit(yaml_visitor);
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
			{
				auto reflected = *handle;
				auto component_typename = reflected.type.name();
				elem.emplace_back(serialize_yaml(reflected)).tag(component_typename);
			
			}
        }

        return yaml::dump(node);
    }

	//template<>
	//string serialize_text(const mono::Behavior& behavior)
	//{
	//	yaml::node node;
	//	yaml::node& mono_data = node.emplace_back();
	//	mono_data.tag(behavior.TypeName());
	//
	//	behavior.GetObject().Visit([&](auto&& key, auto&& value, int)
	//		{
	//			mono_data.emplace_back(serialize_text(value)).tag(key);
	//		});
	//	return yaml::dump(node);
	//}

}