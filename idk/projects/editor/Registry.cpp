#include "pch.h"
#include "Registry.h"
#include <file/FileSystem.h>
#include <util/ioutils.h>
#include <app/Application.h>
#include <proj/ProjectManager.h>
#include <editor/IDE.h>
#include <serialize/yaml.h>

namespace idk
{

    static yaml::node get_yaml(const string& file)
    {
        namespace fs = std::filesystem;

        fs::path recent_path = Core::GetSystem<FileSystem>().GetFullPath("/idk/" + file);
        if (!fs::exists(recent_path))
            return yaml::node{};
        std::ifstream recent_file{ recent_path };
        const auto str = stringify(recent_file);
        auto res = yaml::load(str);
        return res ? *res : yaml::node{};
    }

    string Registry::get(string_view key)
    {
        auto node = get_yaml(name)[string{ key }];
        return node.is_scalar() ? node.as_scalar() : "";
    }

    void Registry::set(string_view key, string value)
    {
        namespace fs = std::filesystem;
        auto node = get_yaml(name);
        node[string{ key }] = value;
        fs::path recent_path = Core::GetSystem<FileSystem>().GetFullPath("/idk/" + name);
        std::ofstream recent_file{ recent_path };
        recent_file << yaml::dump(node);
    }

}