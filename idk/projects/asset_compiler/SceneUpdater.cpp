#include "SceneUpdater.h"
#include <serialize/yaml.inl>
#include <reflect/reflect.inl>
#include <util/ioutils.h>
#include <fstream>

void idk::UpdateScene(string_view full_path)
{
	auto res = yaml::load(stringify(std::ifstream(full_path.data())));
	if (!res)
		return;

	auto& data = *res;
	for (auto& obj : data.as_sequence())
	{
		auto prefab_inst_iter = std::find_if(obj.as_sequence().begin(), obj.as_sequence().end(), 
			[](const yaml::node& o) { return o.tag() == "PrefabInstance"; });
		const bool is_prefab_inst = prefab_inst_iter != obj.as_sequence().end();
		if (!is_prefab_inst)
			continue;

		auto& prefab_inst = *prefab_inst_iter;
		// save default overrides: transform position,rotation,parent; name
		prefab_inst["overrides"] = yaml::sequence_type();
		for (auto& comp : obj)
		{
			if (comp.tag() == "Transform")
			{
				{
					auto& node = prefab_inst["overrides"].emplace_back(yaml::mapping_type());
					node["component_name"] = yaml::scalar_type("Transform");
					node["component_nth"] = yaml::scalar_type("0");
					node["property_path"] = yaml::scalar_type("position");
					node["value"] = comp.as_mapping().at("position");
					node["value"].tag("vec3");
				}
				{
					auto& node = prefab_inst["overrides"].emplace_back(yaml::mapping_type());
					node["component_name"] = yaml::scalar_type("Transform");
					node["component_nth"] = yaml::scalar_type("0");
					node["property_path"] = yaml::scalar_type("rotation");
					node["value"] = comp.as_mapping().at("rotation");
					node["value"].tag("quat");
				}
				{
					auto& node = prefab_inst["overrides"].emplace_back(yaml::mapping_type());
					node["component_name"] = yaml::scalar_type("Transform");
					node["component_nth"] = yaml::scalar_type("0");
					node["property_path"] = yaml::scalar_type("parent");
					node["value"] = comp.as_mapping().at("parent");
					node["value"].tag("Handle<GameObject>");
				}
			}
			else if (comp.tag() == "Name")
			{
				auto& node = prefab_inst["overrides"].emplace_back(yaml::mapping_type());
				node["component_name"] = yaml::scalar_type( "Name" );
				node["component_nth"] = yaml::scalar_type( "0" );
				node["property_path"] = yaml::scalar_type( "name" );
				node["value"] = comp.as_mapping().at("name");
				node["value"].tag("string");
			}
		}

		constexpr auto remove = [](yaml::node& node, const char* cname)
		{
			auto iter = std::find_if(node.as_sequence().begin(), node.as_sequence().end(),
				[cname](const yaml::node& o) { return o.tag() == cname; });
			if (iter != node.as_sequence().end())
				node.as_sequence().erase(iter);
		};

		remove(obj, "Transform");
		remove(obj, "Name");
		remove(obj, "Tag");
		remove(obj, "Layer");
	}

	std::ofstream(full_path.data()) << yaml::dump(data);
}
