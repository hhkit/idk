#include "stdafx.h"
#include "ShaderGraph.h"
#include <regex>
#include <sstream>
#include <filesystem>
namespace fs = std::filesystem;

namespace idk::shadergraph
{

    NodeSignature::NodeSignature(const string& str)
    {
        size_t arrow_pos = str.find_first_of("->");
        if (arrow_pos == std::string::npos)
            return;

        auto str_in = str.substr(0, arrow_pos);
        auto str_out = std::string{ str.begin() + arrow_pos + 2, str.end() };

        std::regex regex{ "\\w+" };
        std::smatch sm;

        while (std::regex_search(str_in, sm, regex))
        {
            auto token = sm.str();
            for (auto& c : token) c = std::toupper(c);
            ins.push_back(ValueType::from_string(token));
            str_in = sm.suffix();
        }
        while (std::regex_search(str_out, sm, regex))
        {
            auto token = sm.str();
            for (auto& c : token) c = std::toupper(c);
            outs.push_back(ValueType::from_string(token));
            str_out = sm.suffix();
        }
    }

    NodeTemplate NodeTemplate::Parse(string_view filename)
    {
        std::ifstream file{ filename };

        vector<NodeSignature> signatures;
        vector<std::string> names;

        string line;
        while (std::getline(file, line))
        {
            NodeSignature sig{ line };
            if (sig.ins.empty() && sig.outs.empty())
            {
                std::regex regex{ "\\w+" };
                std::smatch sm;
                while (std::regex_search(line, sm, regex))
                {
                    names.push_back(sm.str());
                    line = sm.suffix();
                }
                break;
            }
            else
                signatures.push_back(sig);
        }

        // rest of file is code
        std::ostringstream oss;
        oss << file.rdbuf();

        return { signatures, names, oss.str() };
    }

    static NodeTemplate::table _init_table()
    {
        NodeTemplate::table t;

        // todo: use idk::FileSystem
        for (auto& file : fs::recursive_directory_iterator("engine_data\\nodes"))
        {
            if (file.is_directory())
                continue;

            const auto& path = file.path();
            auto path_str = path.string();
            auto node = NodeTemplate::Parse(path_str);

            t.emplace(std::string(path_str.begin() + sizeof("engine_data\\nodes"), path_str.end() - sizeof("node")), node);
        }

        return t;
    }
    const NodeTemplate::table& NodeTemplate::GetTable()
    {
        static table t = _init_table();
        return t;
    }

    size_t NodeTemplate::GetSlotIndex(string_view name) const
    {
        return std::find(names.begin(), names.end(), name) - names.begin();
    }

}