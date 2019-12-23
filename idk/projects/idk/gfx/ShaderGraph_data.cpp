#include "stdafx.h"
#include "ShaderGraph_data.h"
#include <regex>
#include <sstream>
#include <fstream>
#include <file/FileSystem.h>
namespace fs = std::filesystem;

namespace idk::shadergraph
{

    NodeSignature::NodeSignature(const string& str)
    {
        const size_t arrow_pos = str.find_first_of("->");
        if (arrow_pos == std::string::npos)
            return;

        std::string str_in = str.substr(0, arrow_pos);
        auto str_out = std::string{ str.begin() + arrow_pos + 2, str.end() };

        std::regex regex{ "\\w+" };
        std::smatch sm;

        while (std::regex_search(str_in, sm, regex))
        {
            auto token = sm.str();
            for (char& c : token)
                c = static_cast<char>(toupper(c));
            ins.push_back(ValueType::from_string(token));
            str_in = sm.suffix();
        }
        while (std::regex_search(str_out, sm, regex))
        {
            auto token = sm.str();
            for (char& c : token)
                c = static_cast<char>(toupper(c));
            outs.push_back(ValueType::from_string(token));
            str_out = sm.suffix();
        }
    }

    NodeTemplate NodeTemplate::Parse(string_view filename)
    {
        std::ifstream file{ filename };

        vector<NodeSignature> signatures;
        vector<string> names;

        std::string line;
        while (std::getline(file, line))
        {
            NodeSignature sig{ line };
            if (sig.ins.empty() && sig.outs.empty())
            {
                std::regex regex{ "[^,]+" };
                std::smatch sm;
                while (std::regex_search(line, sm, regex))
                {
                    auto str = sm.str();
                    while (str.size() && isspace(str.back())) // strip trailing ws
                        str.pop_back();
                    names.push_back(str);
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



    static NodeTemplate::table t{};
    void NodeTemplate::LoadTable(string_view dir)
    {
        t.clear();
        const auto nodes_dir = Core::GetSystem<FileSystem>().GetFullPath(dir);
        for (auto& file : fs::recursive_directory_iterator(nodes_dir.sv()))
        {
            if (file.is_directory())
                continue;

            const auto& path = file.path();
            auto path_str = path.string();
            auto node = NodeTemplate::Parse(path_str);

            t.emplace(fs::relative(path, nodes_dir.sv()).replace_extension().string(), node);
        }
    }
    const NodeTemplate::table& NodeTemplate::GetTable()
    {
        return t;
    }

    size_t NodeTemplate::GetSlotIndex(string_view name) const
    {
        return std::find(names.begin(), names.end(), name) - names.begin();
    }

}