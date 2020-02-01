#include "stdafx.h"
#include "text.h"

#include <reflect/reflect.inl>
#include <serialize/yaml.inl>

#include <core/GameObject.inl>
#include <scene/Scene.inl>
#include <common/Transform.h>
#include <common/Name.h>
#include <script/MonoBehavior.h>
#include <script/ScriptSystem.h>
#include <script/ManagedObj.inl>
#include <res/ResourceHandle.inl>
#include <prefab/PrefabUtility.h>
#include <prefab/PrefabInstance.h>

#include <ds/span.inl>
#include <ds/result.inl>
namespace idk
{

	template <typename T, typename = void>
	struct has_resize : std::false_type {};
	template <typename T>
	struct has_resize<T, std::void_t<decltype(std::declval<T>().clear())>> : std::true_type {};


	static parse_error parse_yaml(const yaml::node& node, reflect::dynamic& obj); // forward decl

	static parse_error parse_yaml(const yaml::node& node, reflect::uni_container& container)
	{
        parse_error err = parse_error::none;

		if (container.type.is_template<std::array>())
		{
			auto container_iter = container.begin();
			const auto sz = container.size();
			size_t i = 0;
			for (auto iter = node.begin(); iter != node.end() && i < sz; ++iter, ++i)
			{
				auto elem = *container_iter;
                const auto res = parse_yaml(*iter, elem);
				++container_iter;
                if (res != parse_error::none)
                    err = res;
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
                    const auto res = parse_yaml(elem, dyn);
                    if (res != parse_error::none)
                        err = res;
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
                        const auto k_res = parse_text(key, unpacked[0]);
                        const auto v_res = parse_yaml(item_node, unpacked[1]);
                        if (k_res == parse_error::none)
                        {
                            container.add(pair);
                            if (v_res != parse_error::none)
                                err = v_res;
                        }
                        else
                            err = k_res;
                    }
                }
                else
                    err = parse_error::type_cannot_be_parsed;
			}
		}

        return err;
	}

	static parse_error parse_yaml(const yaml::node& node, reflect::dynamic& obj)
	{
        if (node.is_null() && !obj.type.is<mono::ManagedObject>())
            return parse_error::none;
		if (!obj.valid())
		{
            if (node.has_tag())
                obj.swap(reflect::get_type(node.tag()).create());
            else
                return parse_error::invalid_argument;
		}

        if (obj.type.is_enum_type())
        {
            obj = obj.to_enum_value().try_assign(node.as_scalar()).value();
            return parse_error::none;
        }
		else if (obj.type.count() == 0)
		{
#define SERIALIZE_CASE(TYPE) case reflect::typehash<TYPE>() : obj = node.get<TYPE>(); return parse_error::none;
			switch (obj.type.hash())
			{
				SERIALIZE_CASE(bool);
				SERIALIZE_CASE(char);
				SERIALIZE_CASE(uint8_t);
                SERIALIZE_CASE(short);
                SERIALIZE_CASE(uint16_t);
                SERIALIZE_CASE(int);
                SERIALIZE_CASE(uint32_t);
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
                return parse_error::none;
            }
			else if (obj.type.is_container())
			{
				auto cont = obj.to_container();
				return parse_yaml(node, cont);
			}
            else if (obj.type.hash() == reflect::typehash<reflect::dynamic>())
            {
                auto& held = obj.get<reflect::dynamic>();
                return parse_yaml(node, held);
            }
            else if (obj.type.is_template<std::variant>())
            {
                auto dyn = reflect::get_type(node.tag()).create();
                const auto res = parse_yaml(node, dyn);
                if (res == parse_error::none)
                    obj = dyn;
                return res;
            }
			else if (obj.type.hash() != reflect::typehash<mono::ManagedObject>())
                return parse_error::type_cannot_be_parsed;
		}
        else if (obj.type.is_basic_serializable())
        {
            obj = node.as_scalar();
            return parse_error::none;
        }

		vector<const yaml::node*> stack{ &node };
        parse_error err = parse_error::none;
	
		auto generic_visitor = [&](auto&& key, auto&& arg, int depth_change)
		{
			using K = std::decay_t<decltype(key)>;
			using T = std::decay_t<decltype(arg)>;
			while (++depth_change <= 0)
				stack.pop_back();

			if constexpr (std::is_same_v<K, const char*>)
			{
				auto& curr_node = *stack.back();
				if (!curr_node.is_mapping())
					return false;

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
                    err = parse_text(item_node.as_scalar(), arg);
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
                        {
                            const auto res = parse_text(elem.as_scalar(), arg[i]);
                            if (res != parse_error::none)
                                err = res;
                        }
                        else if constexpr(std::is_same_v<std::decay_t<decltype(*arg.begin())>, reflect::dynamic>)
                        {
                            const auto res = parse_yaml(elem, arg[i]);
                            if (res != parse_error::none)
                                err = res;
                        }
						else
						{
							reflect::dynamic d{ arg[i] };
                            const auto res = parse_yaml(elem, d);
                            if (res != parse_error::none)
                                err = res;
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

                            auto k_res = parse_text<ElemK>(elem_name);
                            if (!k_res)
                            {
                                err = k_res.error();
                                continue;
                            }

                            if constexpr (is_basic_serializable_v<ElemV>)
                            {
                                auto v_res = parse_text<ElemV>(elem_val.get<string>());
                                if (!v_res)
                                {
                                    err = v_res.error();
                                    arg.emplace(*k_res, ElemV());
                                }
                                else
                                    arg.emplace(*k_res, *v_res);
                            }
                            else
                            {
                                reflect::dynamic d = arg.emplace(*k_res, ElemV{}).first->second;
                                const auto res = parse_yaml(elem_val, d);
                                if (res != parse_error::none)
                                    err = res;
                            }
						}
						else
						{
							using ElemV = std::decay_t<decltype(*arg.begin())>;
                            if constexpr (is_basic_serializable_v<decltype(arg.begin()->second)>)
                            {
                                auto v_res = parse_text<ElemV>(elem_val.get<string>());
                                if (!v_res)
                                {
                                    err = v_res.error();
                                    arg.emplace(ElemV());
                                }
                                else
                                    arg.emplace(*v_res);
                            }
							else
							{
								ElemV elem_to_emplace{};
                                const auto res = parse_yaml(elem_val, elem_to_emplace);
                                if (res != parse_error::none)
                                    err = res;
								arg.emplace(std::move(elem_to_emplace));
							}
						}
					}
					return false;
				}
				else if constexpr (is_template_v<T, std::variant>)
				{
					auto dyn = reflect::get_type(item_node.tag()).create();
                    err = parse_yaml(item_node, dyn);
                    reflect::dynamic{ arg }.set_variant_value(dyn);
					return false;
				}
                else if constexpr (std::is_same_v<T, reflect::dynamic>) // arg not reflected in ReflectedTypes
                {
                    err = parse_yaml(item_node, arg);
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
				err = parse_error::invalid_argument;
                return false;
			}
		};

		if (obj.is<mono::ManagedObject>())
		{
			obj.get<mono::ManagedObject>() = Core::GetSystem<mono::ScriptSystem>().ScriptEnvironment().Type(node.tag())->Construct();
			obj.get<mono::ManagedObject>().Visit(generic_visitor);
		}
		else
			obj.visit(generic_visitor); // visit

        if (err == parse_error::none)
            obj.on_parse();
        return err;
	}

    template<>
    parse_error parse_text(string_view sv, reflect::dynamic& obj)
    {
        auto res = yaml::load(sv);
        if (res)
            return parse_yaml(*res, obj);
        else
            return res.error();
    }

    monadic::result<reflect::dynamic, parse_error> parse_text(string_view sv, reflect::type type)
    {
        auto obj = type.create();
        const auto res = parse_text(sv, obj);
        if (res == parse_error::none)
            return obj;
        else
            return res;
    }

	template<>
    parse_error parse_text(string_view sv, Scene& scene)
	{
		auto res = yaml::load(sv);
        if (!res)
            return res.error();

        parse_error err = parse_error::none;

		for (auto& elem : *res)
		{
            auto parse_id_res = parse_text<uint64_t>(elem.tag());
            if (!parse_id_res)
                return parse_id_res.error();

			const Handle<GameObject> handle{ *parse_id_res };
			scene.CreateGameObject(handle);

            auto iter = elem.begin();
            handle->SetActive(iter->at("active").get<bool>());

			for (++iter; iter != elem.end(); ++iter)
			{
				const reflect::type type = reflect::get_type(iter->tag());
				reflect::dynamic obj = type.create();
				const auto res2 = parse_yaml(*iter, obj);
				if (res2 != parse_error::none)
					err = res2;

				if (type.is<Transform>())
				{
					auto& trans = *handle->GetComponent<Transform>();
					auto& new_trans = obj.get<Transform>();
					trans.position = new_trans.position;
					trans.rotation = new_trans.rotation;
					trans.scale = new_trans.scale;
					trans.parent = new_trans.parent;
				}
				else if (type.is<Name>())
				{
					handle->GetComponent<Name>()->name = obj.get<Name>().name;
				}
				else if (type.is<PrefabInstance>())
				{
					PrefabUtility::InstantiateSpecific(handle, obj.get<PrefabInstance>());
				}
				else
				{
					handle->AddComponent(obj);
				}
			}
		}

        return err;
	}
}