#pragma once

#include <idk.h>

namespace idk
{
    class Registry
    {
    public:
        Registry(string_view name) : name{ name } {}
        string get(string_view key);
        void set(string_view key, string value);
    private:
        string name;
    };
}